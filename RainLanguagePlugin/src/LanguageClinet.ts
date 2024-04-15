
import { LanguageClient, LanguageClientOptions, ServerOptions, StreamInfo } from 'vscode-languageclient/node'
import * as vsocde from 'vscode'
import stream = require('stream');

let client: LanguageClient;

class TS extends stream.Duplex{
    write(chunk: any, encoding?: BufferEncoding, cb?: (error: Error) => void): boolean;
    write(chunk: any, cb?: (error: Error) => void): boolean;
    write(chunk: unknown, encoding?: unknown, cb?: unknown): boolean {
        console.log("收到数据：")
        console.log(chunk)
        return true;
    }
}

export function StartServer() {
    const serverOptions: ServerOptions = () => {
        const dx = new TS()
        const result: StreamInfo = {
            writer: dx,
            reader: dx
        }
        return Promise.resolve(result)
    }

    const clientOptions: LanguageClientOptions = {
        documentSelector: [{
            language: "雨言"
        }],
        synchronize: {
            fileEvents: vsocde.workspace.createFileSystemWatcher("**/*.rain")
        }
    }
    client = new LanguageClient("雨言", "雨言服务客户端", serverOptions, clientOptions);
    client.start().then(() => {
        console.log("雨言服务客户端：启动")
    }).catch(() => {
        console.log("雨言服务客户端：启动...个屁")
        client = null
    })
}

export function StopServer() {
    if (client) {
        console.log("雨言服务客户端：终止")
        client.stop()
    }
}