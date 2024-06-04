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
    EntryPoint: string
    Timestep: number
    ErrorLevel: number
    Fixed: boolean
}

function IsNUllOrEmpty(value: string): boolean{
    return value == undefined || value == null || value == ""
}

async function CompletionConfiguration(configuration: RainDebugConfiguration) {
    try {
        const data = await fs.promises.readFile(configuration.projectPath + "/.vscode/launch.json", 'utf-8')
        const cfgs = JSON.parse(data).configurations
        cfgs.forEach((element: RainDebugConfiguration) => {
            if (element.type == configuration.type) {
                configuration.projectName = element.ProjectName || configuration.projectName
                configuration.EntryPoint = element.EntryPoint || configuration.EntryPoint
                configuration.Timestep = element.Timestep || configuration.Timestep
                configuration.ErrorLevel = element.ErrorLevel || configuration.ErrorLevel
                configuration.Fixed = element.Fixed || configuration.Fixed
            }
        });
    } catch (error) {
        vscode.window.showErrorMessage("解析配置文件时出现错误：" + error)
     }
     if (IsNUllOrEmpty(configuration.projectName)) {
         vscode.window.showInformationMessage("获取当前工程名失败，将使用工作空间文件夹名作为工程名")
         configuration.projectName = path.dirname(configuration.projectPath)
    }
    configuration.detectorName = "RainDebuggerDetector.dll";
}

let lastPicked: string = null;

async function pickPID(injector: string) {
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
                if (lastPicked != null) {
                    let last = items.findIndex(value => value.detail == lastPicked)
                    if (last >= 0) {
                        let [item] = items.splice(last, 1)
                        items.unshift(item)
                    }
                }
                vscode.window.showQuickPick(items, {
                    matchOnDescription: true,
                    matchOnDetail: true,
                    placeHolder: "选择要附加的进程"
                }).then((item: ProcessInfoItem | undefined) => {
                    if (item) {
                        console.log
                        console.log("附加到进程：" + item.label);
                        if (item.detail) {
                            console.log("进程详情:" + item.detail);
                        }
                        lastPicked = item.detail
                        resolve(item.pid)
                    } else {
                        reject()
                    }
                })
            } else if (items.length > 0) {
                lastPicked = items[0].detail
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
        const cmd = `"${injector}" ${pid} "${detectorPath}" "${detectorName}" "${projectPath}" "${projectName}"`
        cp.exec(cmd, { encoding: "buffer" }, (error, stdout, stderr) => {
            const result = iconv.decode(stdout, "cp936").trim();
            const port = parseInt(result, 10);
            if (Number.isNaN(port)) {
                vscode.window.showErrorMessage("调试器dll注入失败:" + result)
                reject()
            }
            else {
                resolve(port)
            }
        }).on('error', error => {
            reject()
        })
    })
}

async function Connect(port:number) {
    return new Promise<net.Socket>((resolve, reject) => {
        if (port < 1000) {
            vscode.window.showErrorMessage("调试器链接错误，错误码：" + port)
            reject()
        } else {
            const client = net.connect({
                port: port,
                host: "127.0.0.1"
            })
            client.on('connect', () => {
                resolve(client)
            });
            client.on('error', error => {
                console.log("调试器链接失败：" + error.message);
                reject()
            }).on('close', (hasErr) => {
                console.log("关闭 client");
            })
        }
    })
}

async function GetLaunchParam(configuration: RainDebugConfiguration) {
    const exeFile = configuration.detectorPath + "/RainLauncher.exe";
    const name = configuration.projectName
    const errLvl = configuration.ErrorLevel || 4
    const entry = configuration.EntryPoint || "Main"
    const timestep = configuration.Timestep || 1
    
    let result: string[] = [];
    result.push(exeFile);
    result.push("-path")
    result.push(configuration.projectPath)
    result.push("-name")
    result.push(name)
    result.push("-errorlevel")
    result.push(errLvl.toString())
    result.push("-entry")
    result.push(entry)
    result.push("-timestep")
    result.push(timestep.toString())
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

export let debuggedProcess: cp.ChildProcess = null;
export let currentOutputChannel: vscode.OutputChannel = null;
export let client: net.Socket = null;
let statusBarItem: vscode.StatusBarItem = null;
let intervalId: NodeJS.Timer = null;
let compileTime: number = 0;
let rainCompileResolve: (value: unknown) => void = null;
let rainCompileReject: () => void = null;
function ListenCPReadyDebug(data: Buffer) {
    const msg = data.toString()
    if (msg.includes("<ready to connect debugger>")) {
        console.log(msg.replace("<ready to connect debugger>", ""))
        let output = currentOutputChannel
        
        debuggedProcess.stdout.removeListener('data', ListenCPReadyDebug)
        debuggedProcess.removeListener('exit', ListenCPExit);
        debuggedProcess.stdout.on('data', (data: Buffer) => {
            output.append(data.toString())
        });
        rainCompileResolve(null)
        rainCompileResolve = null;
        rainCompileReject = null;
    } else if (msg.includes("<compilation failure>")) {
        let output = currentOutputChannel
        output.append(msg.replace("<compilation failure>", ""))
        
        debuggedProcess.stdout.removeListener('data', ListenCPReadyDebug)
        debuggedProcess.stdout.on('data', (data: Buffer) => {
            output.append(data.toString())
        });
    } else {
        console.log(data.toString())
    }
}
function ListenCPExit() {
    statusBarItem.dispose()
    statusBarItem = null
    clearInterval(intervalId)
    intervalId = null
    rainCompileReject()
    rainCompileResolve = null;
    rainCompileReject = null;
}
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
        if (debuggedProcess != null && !debuggedProcess.killed && debuggedProcess.exitCode == null) {
            debuggedProcess.kill()
            debuggedProcess = null
        }
        await CompletionConfiguration(configuration)
        if (configuration.Fixed) {
            configuration.detectorPath = this.context.extensionUri.fsPath + "/bin/fixed/";
        } else {
            configuration.detectorPath = this.context.extensionUri.fsPath + "/bin/float/";
        }
        if (configuration.noDebug) {
            const launchParams = await GetLaunchParam(configuration)
            const outputChannel = GetOutputChannel(configuration.projectName + "[雨言]");
            debuggedProcess = cp.execFile(launchParams[0], launchParams.slice(1)).on('error', error => {
                vscode.window.showErrorMessage(error.message)
            })
            debuggedProcess.stdout.on('data', (data: Buffer) => {
                outputChannel.append(data.toString())
            });
        } else {
            const injector = `${configuration.detectorPath}/RainDebuggerInjector.exe`
            switch (configuration.type) {
                case "雨言调试运行": {
                    if (intervalId != null) {
                        clearInterval(intervalId)
                    }
                    if (statusBarItem != null) {
                        statusBarItem.dispose()
                    }
                    statusBarItem = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Left);
                    statusBarItem.show()
                    compileTime = 0;
                    statusBarItem.text = `${configuration.projectName} 编译中，已用时${compileTime}s`
                    intervalId = setInterval(() => {
                        compileTime++
                        statusBarItem.text = `${configuration.projectName} 编译中，已用时${compileTime}s`
                    }, 1000);
                    
                    const launchParams = await GetLaunchParam(configuration)
                    launchParams.push("-debug")
                    currentOutputChannel = GetOutputChannel(configuration.projectName + "[调试]");

                    debuggedProcess = cp.execFile(launchParams[0], launchParams.slice(1)).on('error', error => {
                        vscode.window.showErrorMessage(error.message)
                    })
                    debuggedProcess.stdout.addListener('data', ListenCPReadyDebug)
                    debuggedProcess.addListener('exit', ListenCPExit)

                    await new Promise((resolve, reject) => {
                        rainCompileResolve = resolve;
                        rainCompileReject = reject;
                    });
                    clearInterval(intervalId)
                    intervalId = null
                    statusBarItem.dispose()
                    statusBarItem = null

                    if (debuggedProcess.pid) {
                        const port = await GetDetectorPort(injector, debuggedProcess.pid, configuration.detectorPath, configuration.detectorName, configuration.projectPath, configuration.projectName)
                        if (port > 0) {
                            client = await Connect(port);
                            return configuration
                        }
                        console.log("端口获取失败")
                    } else {
                        console.log("子进程id获取失败")
                    }
                    debuggedProcess.stdin.write('n')
                    debuggedProcess.stdin.end();
                    return undefined
                }
                case "雨言附加到进程": {
                    currentOutputChannel = GetOutputChannel(configuration.projectName + `[附加到进程]`);

                    const pid = await pickPID(injector)
                    const port = await GetDetectorPort(injector, pid, configuration.detectorPath, configuration.detectorName, configuration.projectPath, configuration.projectName)
                    client = await Connect(port)
                    return configuration
                }
            }
        }
    }
}