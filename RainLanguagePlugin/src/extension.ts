import * as vscode from "vscode";
import FormatProvider from './formatterProvider'
import { InlineDebugAdapterFactory } from "./debugger/DebugAdapterFactory";
import { RainDebugConfigurationProvider } from "./debugger/DebugConfigurationProvider";
import { KernelStateViewProvider } from "./KernelStateViewProvider";
import { readFile } from "fs";
import * as rainLanguageClient from "./LanguageClinet";
import { RainEvaluatableExpressionProvider } from "./EvaluatableExpressionProvider";

export let kernelStateViewProvider: KernelStateViewProvider;

export async function activate(context: vscode.ExtensionContext) {
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
            const uri = vscode.Uri.parse("rain-language:kernel.rain")
            const doc = await vscode.workspace.openTextDocument(uri)
            vscode.window.showTextDocument(doc, { preview: true })
        }),
        vscode.workspace.registerTextDocumentContentProvider("rain-language", {
            provideTextDocumentContent: function (uri: vscode.Uri, token: vscode.CancellationToken): vscode.ProviderResult<string> {
                return new Promise<string>((resolve, reject) => {
                    const path = context.extensionPath + "/" + uri.path;
                    readFile(path, (error, data) => {
                        if (error) {
                            reject(error)
                        } else {
                            resolve(data.toString())
                        }
                    })
                })
            }
        })
    );
    rainLanguageClient.StartServer(context);
}
export async function deactivate() {
    rainLanguageClient.StopServer();
}