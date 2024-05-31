
import * as vscode from 'vscode'

const tokenTypes = ['namespace', 'type', 'enum', 'struct', 'class', 'interface', 'function', 'method', 'macro', 'variable', 'parameter', 'enumMember']
export const legend = new vscode.SemanticTokensLegend(tokenTypes)

export class SemanticTokenProvider implements vscode.DocumentSemanticTokensProvider {
    provideDocumentSemanticTokens(document: vscode.TextDocument, token: vscode.CancellationToken): vscode.ProviderResult<vscode.SemanticTokens> {
        const builder = new vscode.SemanticTokensBuilder(legend)
        builder.push(1, 1, 2, 0);
        return builder.build()
    }
}