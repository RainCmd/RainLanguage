'use strict';

import * as vscode from "vscode";
import FormatProvider from './formatterProvider'
import { InlineDebugAdapterFactory } from "./DebugAdapterFactory";
import { RainDebugConfigurationProvider } from "./DebugConfigurationProvider";


export async function activate(context: vscode.ExtensionContext) {
    const documentSelector: vscode.DocumentSelector = {
        language: '雨言',
    };
    context.subscriptions.push(
        vscode.languages.registerDocumentRangeFormattingEditProvider(documentSelector, new FormatProvider()),
        vscode.languages.registerOnTypeFormattingEditProvider(documentSelector, new FormatProvider(), '\n'),
        vscode.debug.registerDebugConfigurationProvider("雨言附加到进程", new RainDebugConfigurationProvider(context)),
        vscode.debug.registerDebugAdapterDescriptorFactory("雨言附加到进程", new InlineDebugAdapterFactory()),
        vscode.commands.registerCommand('cmd.雨言远程调试', () => {
            vscode.debug.startDebugging(undefined, {
                type: "雨言附加到进程",
                name: "雨言远程调试",
                request: "attach",
                projectName: ""
            })
    }));
}
export async function deactivate() {
}