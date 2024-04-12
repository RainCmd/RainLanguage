import * as vscode from "vscode";
import FormatProvider from './formatterProvider'
import { InlineDebugAdapterFactory } from "./DebugAdapterFactory";
import { RainDebugConfigurationProvider } from "./DebugConfigurationProvider";
import { KernelStateViewProvider } from "./KernelStateViewProvider";

export let kernelStateViewProvider: KernelStateViewProvider;

export async function activate(context: vscode.ExtensionContext) {
    const documentSelector: vscode.DocumentSelector = {
        language: '雨言',
    };
    kernelStateViewProvider = new KernelStateViewProvider(context.extensionUri)
    context.subscriptions.push(
        vscode.languages.registerDocumentRangeFormattingEditProvider(documentSelector, new FormatProvider()),
        vscode.languages.registerOnTypeFormattingEditProvider(documentSelector, new FormatProvider(), '\n'),
        vscode.languages.registerEvaluatableExpressionProvider(documentSelector, {
            provideEvaluatableExpression(document, position, token) {
                return new vscode.EvaluatableExpression(new vscode.Range(position, position),
                    document.fileName + " " + (position.line + 1) + " " + position.character)
            },
        }),

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

        vscode.window.registerWebviewViewProvider("RainKernelState", kernelStateViewProvider)
    );
}
export async function deactivate() {
}