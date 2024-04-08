import * as vscode from 'vscode'
import * as fs from 'fs'
import * as path from 'path'
import * as cp from 'child_process'
import * as iconv from 'iconv-lite';
import { receiveMessageOnPort } from 'worker_threads';

interface ProcessInfoItem extends vscode.QuickPickItem {
    pid: number;
}

export interface RainAttachDebugConfiguration extends vscode.DebugConfiguration{
    detectorPath: string,
    detectorName: string,
    projectPath: string,
    projectName: string
}

function IsNUllOrEmpty(value: string): boolean{
    return value == undefined || value == null || value == ""
}

async function GetProjectName(projectPath: string, name: string): Promise<string> {
    if (IsNUllOrEmpty(name)) {
        try {
            const data = await fs.promises.readFile(projectPath + "/.vscode/launch.json", 'utf-8')
            const cfgs = JSON.parse(data).configurations
            cfgs.forEach((element: { type: string; projectName: string }) => {
                if (element.type == "雨言附加到进程") {
                    name = element.projectName
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

export class RainAttachDebugConfigurationProvider implements vscode.DebugConfigurationProvider {
    constructor(protected context: vscode.ExtensionContext) { }
    async resolveDebugConfiguration(folder: vscode.WorkspaceFolder, configuration: RainAttachDebugConfiguration): Promise<vscode.DebugConfiguration> {
        if (vscode.workspace.workspaceFolders.length > 0) {
            const workspaceFolder = vscode.workspace.workspaceFolders[0];
            configuration.projectPath = workspaceFolder.uri.fsPath;
        } else if (folder != undefined && folder != null) {
            configuration.projectPath = folder.uri.fsPath;
        } else return undefined;
        configuration.projectName = await GetProjectName(configuration.projectPath, configuration.projectName)
        configuration.detectorPath = this.context.extensionUri.fsPath;
        configuration.detectorName = "RainDebuggerDetector.dll";
        const injector = this.context.extensionUri.fsPath + "/RainDebuggerInjector.exe"
        const pid = await pickPID(injector)
        //todo 将dector注入目标进程
        return configuration
    }
}