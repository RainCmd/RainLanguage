import * as vscode from 'vscode'
import * as fs from 'fs'
import * as path from 'path'
import * as cp from 'child_process'
import * as iconv from 'iconv-lite';
import * as net from 'net'

interface ProcessInfoItem extends vscode.QuickPickItem {
    pid: number;
}

export interface RainDebugConfiguration extends vscode.DebugConfiguration{
    detectorPath: string
    detectorName: string
    projectPath: string
    projectName: string
    socket: net.Socket
    output: vscode.OutputChannel
}

function IsNUllOrEmpty(value: string): boolean{
    return value == undefined || value == null || value == ""
}

async function GetProjectName(projectPath: string, name: string, type: string): Promise<string> {
    if (IsNUllOrEmpty(name)) {
        try {
            const data = await fs.promises.readFile(projectPath + "/.vscode/launch.json", 'utf-8')
            const cfgs = JSON.parse(data).configurations
            cfgs.forEach((element: { type: string; ProjectName: string }) => {
                if (element.type == type) {
                    name = element.ProjectName
                }
            });
        } catch (error) { }
        if (IsNUllOrEmpty(name)) {
            vscode.window.showInformationMessage("获取当前工程名失败，将使用工作空间文件夹名作为工程名")
            name = path.dirname(projectPath)
        }
    }
    return name
}

async function pickPID(injector: string, output: vscode.OutputChannel) {
    return new Promise<number>((resolve, reject) => {
        cp.exec(injector, { encoding: "buffer" }, (error, stdout, stderr) => {
            const arr = iconv.decode(stdout, "cp936").split("\r\n");
            const items: ProcessInfoItem[] = [];
            let cur: ProcessInfoItem = null
            for (let index = 0; index < arr.length; index++) {
                const element = arr[index];
                if (element.startsWith("id")) {
                    if (cur != null) items.push(cur);
                    cur = {
                        pid: Number(element.substring(2)),
                        label: null
                    }
                } else if (element.startsWith("path")) {
                    cur.detail = element.substring(4)
                    cur.label = `[${cur.pid}] ${path.basename(cur.detail)}`
                } else if (element.startsWith("title")) {
                    cur.description = element.substring(5)
                } else if (element.startsWith("name")) {
                    // 这里的name大多时候和cur.label相同，所以就不加了
                    // if (IsNUllOrEmpty(cur.description)) {
                    //     cur.description = element.substring(4);
                    // }
                }
            }
            if (cur != null) items.push(cur);
            if (items.length > 1) {
                vscode.window.showQuickPick(items, {
                    matchOnDescription: true,
                    matchOnDetail: true,
                    placeHolder: "选择要附加的进程"
                }).then((item: ProcessInfoItem | undefined) => {
                    if (item) {
                        output.appendLine("附加到进程：" + item.label);
                        if (item.detail) {
                            output.appendLine("进程详情:" + item.detail);
                        }
                        resolve(item.pid)
                    } else {
                        reject()
                    }
                })
            } else if (items.length > 0) {
                resolve(items[0].pid)
            } else {
                vscode.window.showErrorMessage("没有可以附加的进程")
                reject();
            }
        }).on("error", error => reject)
    })
}

async function GetDetectorPort(injector: string, pid: number, detectorPath: string, detectorName: string, projectPath: string, projectName: string) {
    return new Promise<number>((resolve, reject) => {
        cp.exec(`${injector} ${pid} ${detectorPath} ${detectorName} ${projectPath} ${projectName}`, { encoding: "buffer" }, (error, stdout, stderr) => {
            const port = Number(iconv.decode(stdout, "cp936").trim())
            if (port > 0) {
                resolve(port)
            } else {
                reject(port)
            }
        }).on('error', error => {
            reject()
        })
    })
}

async function Connect(port:number, output: vscode.OutputChannel) {
    return new Promise<net.Socket>((resolve, reject) => {
        const client = net.connect({
            port: port,
            host: "localhost",
            family: 4
        })
        client.on('connect', () => {
            output.appendLine("调试器链接成功")
            resolve(client)
        });
        client.on('error', error => {
            output.appendLine("调试器链接失败：" + error.message);
            reject()
        })
    })
}

async function GetEntryAndTimestep(projectPath: string, entry: string, timestep: number, type: string): Promise<{ entry: string, timestep: number }> {
    try {
        const data = await fs.promises.readFile(projectPath + "/.vscode/launch.json", 'utf-8')
        const cfgs = JSON.parse(data).configurations
        cfgs.forEach((element: { type: string; EntryPoint: string, Timesnap: number }) => {
            if (element.type == type) {
                entry = element.EntryPoint;
                timestep = element.Timesnap;
            }
        });
    } catch (error) { }
    return {
        entry: entry,
        timestep: timestep
    }
}


async function GetLaunchParam(configuration: RainDebugConfiguration) {
    const exeFile = configuration.detectorPath + "/RainLauncher.exe";
    const name = configuration.projectName
    const cfg = await GetEntryAndTimestep(configuration.projectPath, "Main", 100, "雨言调试运行")

    let result: string[] = [];
    result.push(exeFile);
    result.push("-path")
    result.push(configuration.detectorPath)
    result.push("-name")
    result.push(name)
    result.push("-entry")
    result.push(cfg.entry)
    result.push("-timestep")
    result.push(cfg.timestep.toString())
    return result
}

const outputChannelMap = {}
function GetOutputChannel(name: string):vscode.OutputChannel {
    if (outputChannelMap[name]) {
        const result = outputChannelMap[name] as vscode.OutputChannel
        result.clear()
        result.show()
        return result
    } else {
        const result = vscode.window.createOutputChannel(name);
        outputChannelMap[name] = result;
        result.show();
        return result;
    }
}

let childProcess: cp.ChildProcess = null;
export class RainDebugConfigurationProvider implements vscode.DebugConfigurationProvider {
    constructor(protected context: vscode.ExtensionContext) { }
    async resolveDebugConfiguration(folder: vscode.WorkspaceFolder, configuration: RainDebugConfiguration): Promise<vscode.DebugConfiguration> {
        if (vscode.workspace.workspaceFolders.length > 0) {
            const workspaceFolder = vscode.workspace.workspaceFolders[0];
            configuration.projectPath = workspaceFolder.uri.fsPath;
        } else if (folder != undefined && folder != null) {
            configuration.projectPath = folder.uri.fsPath;
        } else return undefined;
        if (configuration.noDebug) {
            configuration.type = "雨言调试运行"
        }
        if (childProcess != null && !childProcess.killed && childProcess.exitCode == null) {
            childProcess.kill()
            childProcess = null
        }
        configuration.projectName = await GetProjectName(configuration.projectPath, configuration.projectName, configuration.type)
        configuration.detectorPath = this.context.extensionUri.fsPath;
        configuration.detectorName = "RainDebuggerDetector.dll";
        if (configuration.noDebug) {
            const launchParams = await GetLaunchParam(configuration)
            const outputChannel = GetOutputChannel(configuration.projectName + "[雨言]");
            childProcess = cp.execFile(launchParams[0], launchParams.slice(1)).on('error', error => {
                vscode.window.showErrorMessage(error.message)
            })
            childProcess.stdout.on('data', (data: Buffer) => {
                outputChannel.append(data.toString())
            });
        } else {
            switch (configuration.type) {
                case "雨言调试运行": {
                    const launchParams = await GetLaunchParam(configuration)
                    launchParams.push("-debug")
                    const outputChannel = GetOutputChannel(configuration.projectName + "[调试]");
                    configuration.output = outputChannel

                    childProcess = cp.execFile(launchParams[0], launchParams.slice(1)).on('error', error => {
                        vscode.window.showErrorMessage(error.message)
                    })
                    childProcess.stdout.on('data', (data: Buffer) => {
                       outputChannel.append(data.toString())
                    });
                    if (childProcess.pid) {
                        const injector = this.context.extensionUri.fsPath + "/RainDebuggerInjector.exe"
                        const port = await GetDetectorPort(injector, childProcess.pid, configuration.detectorPath, configuration.detectorName, configuration.projectPath, configuration.projectName)
                        configuration.socket = await Connect(port, outputChannel);
                        childProcess.stdin.write('y')
                        childProcess.stdin.end();
                        return configuration
                    } else {
                        outputChannel.appendLine("子进程id获取失败")
                        childProcess.stdin.write('n')
                        childProcess.stdin.end();
                        return undefined
                    }
                }
                case "雨言附加到进程": {
                    const outputChannel = GetOutputChannel(configuration.projectName + `[附加到进程]`);
                    configuration.output = outputChannel

                    const injector = this.context.extensionUri.fsPath + "/RainDebuggerInjector.exe"
                    const pid = await pickPID(injector, outputChannel)
                    const port = await GetDetectorPort(injector, pid, configuration.detectorPath, configuration.detectorName, configuration.projectPath, configuration.projectName)
                    configuration.socket = await Connect(port, outputChannel)
                    return configuration
                }
            }
        }
    }
}