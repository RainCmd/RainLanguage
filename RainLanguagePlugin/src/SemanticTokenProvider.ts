
import * as vscode from 'vscode'
import { GetSemanticTokens } from './LanguageClinet'

const tokenTypes = ['namespace', 'type', 'enum', 'struct', 'class', 'interface', 'function', 'method', 'enumMember', 'variable', 'parameter', 'enumMember', 'operator']
export const legend = new vscode.SemanticTokensLegend(tokenTypes)
interface TokenType{
    type: number
    ranges: TokenRange[]
}
interface TokenRange{
    line: number
    index: number
    length: number
}
export class SemanticTokenProvider implements vscode.DocumentSemanticTokensProvider {
    async provideDocumentSemanticTokens(document: vscode.TextDocument, token: vscode.CancellationToken): Promise<vscode.SemanticTokens> {
        const builder = new vscode.SemanticTokensBuilder(legend)
        const tokens = await GetSemanticTokens(document, token)
        if (tokens) {
            tokens.forEach((type: TokenType) => {
                type.ranges.forEach(element => {
                    builder.push(element.line, element.index, element.length, type.type)
                });
            });
        }
        return builder.build()
    }
}