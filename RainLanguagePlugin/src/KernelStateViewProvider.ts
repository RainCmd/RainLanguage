import EventEmitter = require("events")
import * as vscode from 'vscode'

export interface KernelStateMsg{
    task: number
    string: number
    entity: number
    handle: number
    heap: number
}
export class KernelStateViewProvider extends EventEmitter implements vscode.WebviewViewProvider {
    private view: vscode.WebviewView
    constructor(private extensionUri: vscode.Uri) {
        super()
     }
    resolveWebviewView(webviewView: vscode.WebviewView, context: vscode.WebviewViewResolveContext<unknown>, token: vscode.CancellationToken): void | Thenable<void> {
        this.view = webviewView
        webviewView.webview.options = {
            enableScripts: true,
            localResourceRoots: [this.extensionUri]
        }

        this.view.onDidChangeVisibility(() => { 
            this.emit('ChangeVisibility', this.view.visible)
        })
        const viewScriptUrl = this.view.webview.asWebviewUri(vscode.Uri.joinPath(this.extensionUri, 'media', 'KernelStateView.js'))
        webviewView.webview.html = `
            <html>
            <head>
                <script src="https://cdn.staticfile.net/Chart.js/3.9.1/chart.js"></script>
                <style type="text/css">      
                html, body, #container { 
                    width: 100%; margin: 0; padding: 0; 
                } 
                </style>
            </head>
            <body>  
                <canvas id="container"></canvas>
                <script src = ${viewScriptUrl} > </script>
            </body>
            </html>
        `
    }
    public Show() {
        if (this.view) {
            this.view.webview.postMessage({
                type: 'show'
            })
            this.view.show()
        }
    }
    public RecvData(data: KernelStateMsg) {
        if (this.view) {
            this.view.webview.postMessage({
                type: 'data',
                data: data
            })
        }
    }
}