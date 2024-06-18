import * as vscode from "vscode";
import FormatProvider from './formatterProvider'
import { InlineDebugAdapterFactory } from "./debugger/DebugAdapterFactory";
import { RainDebugConfigurationProvider } from "./debugger/DebugConfigurationProvider";
import { KernelStateViewProvider } from "./KernelStateViewProvider";
import { readFile } from "fs";
import * as rainLanguageClient from "./LanguageClinet";
import { RainEvaluatableExpressionProvider } from "./EvaluatableExpressionProvider";
import { SemanticTokenProvider, legend } from "./SemanticTokenProvider";

export const extensionDebug = false
export let kernelStateViewProvider: KernelStateViewProvider;

export async function activate(context: vscode.ExtensionContext) {
    vscode.commands.executeCommand("setContext", "extensionDebug", extensionDebug)
    const documentSelector: vscode.DocumentSelector = {
        language: '雨言',
    };
    kernelStateViewProvider = new KernelStateViewProvider(context.extensionUri)
    context.subscriptions.push(
        vscode.languages.registerDocumentRangeFormattingEditProvider(documentSelector, new FormatProvider()),
        vscode.languages.registerOnTypeFormattingEditProvider(documentSelector, new FormatProvider(), '\n', ';'),
        vscode.languages.registerEvaluatableExpressionProvider(documentSelector, new RainEvaluatableExpressionProvider()),

        vscode.debug.registerDebugConfigurationProvider("雨言调试运行", new RainDebugConfigurationProvider(context)),
        vscode.debug.registerDebugAdapterDescriptorFactory("雨言调试运行", new InlineDebugAdapterFactory()),
        vscode.commands.registerCommand("cmd.雨言调试", () => {
            vscode.debug.startDebugging(undefined, {
                type: "雨言调试运行",
                name: "调试",
                request: "launch"
            })
        }),

        vscode.debug.registerDebugConfigurationProvider("雨言附加到进程", new RainDebugConfigurationProvider(context)),
        vscode.debug.registerDebugAdapterDescriptorFactory("雨言附加到进程", new InlineDebugAdapterFactory()),
        vscode.commands.registerCommand('cmd.附加到进程', () => {
            vscode.debug.startDebugging(undefined, {
                type: "雨言附加到进程",
                name: "附加到进程",
                request: "attach"
            })
        }),

        vscode.window.registerWebviewViewProvider("RainKernelState", kernelStateViewProvider),

        vscode.commands.registerCommand('cmd.核心库定义', async () => {
            const path = "kernel.rain"
            const filePath = context.extensionPath + "/" + path;
            readFile(filePath, (error, data) => {
                if (error) {
                    console.log(error)
                } else {
                    RegistRainLanguagePreviewDoc(path, data.toString())
                    vscode.window.showTextDocument(vscode.Uri.parse("rain-language:" + path))
                }
            })
        }),
        vscode.workspace.registerTextDocumentContentProvider("rain-language", {
            provideTextDocumentContent: function (uri: vscode.Uri) { return langugaePreviewDoc.get(uri.path) } 
        }),

        vscode.commands.registerCommand('cmd.debug.重启雨言服务', () => rainLanguageClient.RestartServer(context)),

        vscode.languages.registerDocumentSemanticTokensProvider(documentSelector,new SemanticTokenProvider(), legend)
    );
    await rainLanguageClient.StartServer(context);
}
export async function deactivate() {
    rainLanguageClient.StopServer();
}

const langugaePreviewDoc = new Map<string, string>()
export function RegistRainLanguagePreviewDoc(path: string, content: string) {
    const uri = vscode.Uri.parse(path)
    if (uri.scheme == "rain-language")
        langugaePreviewDoc.set(uri.path, content)
}