'use strict';

import * as vscode from "vscode";
import FormatProvider from './formatterProvider'


export async function activate(context: vscode.ExtensionContext) {
    const documentSelector: vscode.DocumentSelector = {
        language: '雨言',
    };
    context.subscriptions.push(vscode.languages.registerDocumentRangeFormattingEditProvider(documentSelector,
        new FormatProvider()
    ));
    context.subscriptions.push(vscode.languages.registerOnTypeFormattingEditProvider(documentSelector,
        new FormatProvider(),
        '\n'));
    context.subscriptions.push(vscode.commands.registerCommand('cmd.雨言远程调试', (arg: string) => {
        vscode.window.showInformationMessage("啊啊啊啊");
        vscode.window.showInformationMessage(arg);
    }, "这是参数？"));
}
export async function deactivate() {
}