import * as vscode from 'vscode'
import { MatchPairingCharacter } from './formatterProvider'

const cc__ = '_'.charCodeAt(0)
const cc_a = 'a'.charCodeAt(0)
const cc_z = 'z'.charCodeAt(0)
const cc_A = 'A'.charCodeAt(0)
const cc_Z = 'Z'.charCodeAt(0)
const cc_0 = '0'.charCodeAt(0)
const cc_9 = '9'.charCodeAt(0)

function IsLetterOrNumber(cc:number) {
    if (cc__ == cc || cc > 128) return true
    if (cc >= cc_a && cc < cc_z) return true;
    if (cc >= cc_A && cc < cc_Z) return true;
    if (cc >= cc_0 && cc < cc_9) return true;
    return false
}
function MatchToken(line: string, index: number) {
    if (!IsLetterOrNumber(line.charCodeAt(index))) return {
        start: index,
        end: index + 1
    }
    let start = index - 1
    while (start >= 0 && IsLetterOrNumber(line.charCodeAt(start))) {
        start--
    }
    let end = index + 1
    while (end < line.length && IsLetterOrNumber(line.charCodeAt(start))) {
        end++
    }
    return {
        start: start + 1,
        end: end
    }
}
export class RainEvaluatableExpressionProvider implements vscode.EvaluatableExpressionProvider{
    private CreateExpression(line: number, start: number, end: number, file: string) {
        return new vscode.EvaluatableExpression(new vscode.Range(
            new vscode.Position(line, start),
            new vscode.Position(line, end)
        ), `${file} ${line + 1} ${start}`)
    }
    provideEvaluatableExpression(document: vscode.TextDocument, position: vscode.Position, token: vscode.CancellationToken): vscode.ProviderResult<vscode.EvaluatableExpression> {
        const line = document.lineAt(position.line)
        let pos = MatchPairingCharacter(line.text, 0)
        if (position.character >= pos) {
            while (pos >= 0) {
                let end = MatchPairingCharacter(line.text, pos + 1, line.text[pos])
                if (position.character < pos) {
                    break
                } else if (position.character <= end) {
                    return this.CreateExpression(position.line, pos, end + 1, document.fileName)
                }
                pos = MatchPairingCharacter(line.text, end + 1)
            }
        }
        const range = MatchToken(line.text, position.character)
        return this.CreateExpression(position.line, range.start, range.end, document.fileName)
    }
}