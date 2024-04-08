'use strict';
Object.defineProperty(exports, "__esModule", { value: true });
exports.deactivate = exports.activate = void 0;
const vscode = require("vscode");
const formatterProvider_1 = require("./formatterProvider");
async function activate(context) {
    const documentSelector = {
        language: '雨言',
    };
    context.subscriptions.push(vscode.languages.registerDocumentRangeFormattingEditProvider(documentSelector, new formatterProvider_1.default()));
    context.subscriptions.push(vscode.languages.registerOnTypeFormattingEditProvider(documentSelector, new formatterProvider_1.default(), '\n'));
    context.subscriptions.push(vscode.commands.registerCommand('cmd.雨言远程调试', (arg) => {
        vscode.window.showInformationMessage("啊啊啊啊");
        vscode.window.showInformationMessage(arg);
    }, "这是参数？"));
}
exports.activate = activate;
async function deactivate() {
}
exports.deactivate = deactivate;
//# sourceMappingURL=extension.js.map