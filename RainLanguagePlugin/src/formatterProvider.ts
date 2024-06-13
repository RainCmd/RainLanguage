
import { DocumentRangeFormattingEditProvider, OnTypeFormattingEditProvider, FormattingOptions, CancellationToken, TextEdit, TextDocument, Range, Position, ProviderResult } from 'vscode';

export function MatchPairingCharacter(line: string, start: number, char?: string): number{
    if (char == null) {
        let ss = line.indexOf("\'", start)
        let ds = line.indexOf("\"", start)
        if (ss < 0) return ds
        else if (ds < 0) return ss
        else if (ss < ds) return ss
        else return ds
    } else {
        let pos = line.indexOf(char, start)
        if (pos < 0) return line.length
        if (pos > 0 && line[pos - 1] == '\\') return this.MatchPairingCharacter(line, pos + 1, char)
        return pos
    }
}

export default class FormatProvider implements DocumentRangeFormattingEditProvider, OnTypeFormattingEditProvider {
    public async provideDocumentRangeFormattingEdits(document: TextDocument, range: Range, options: FormattingOptions, token: CancellationToken): Promise<TextEdit[]> {
        let results: TextEdit[] = [];

        for (let index = range.start.line; index <= range.end.line; index++) {
            const line = document.lineAt(index);
            const newText = this.FormatLine(line.text);
            if (line.text != newText) {
                results.push(new TextEdit(line.range, newText));
            }
        }
        return results;
    }
    public async provideOnTypeFormattingEdits(document: TextDocument, position: Position, ch: string, options: FormattingOptions, token: CancellationToken): Promise<TextEdit[]> {
        const line = document.lineAt(ch == '\n' ? position.line - 1 : position.line);
        const newText = this.FormatLine(line.text);
        if (line.text != newText) {
            return [
                new TextEdit(line.range, this.FormatLine(line.text))
            ];
        } else {
            return [];
        }
    }
    private FormatLine(line: string): string {
        const end = line.search(/\/\//gi)
        let annotation = ""
        if (end >= 0) {
            annotation = line.substring(end)
            line = line.substring(0, end)
        }
        let result = ""
        let start = 0
        let pos = MatchPairingCharacter(line, 0)
        while (pos >= 0) {
            let char = line[pos]
            pos += 1
            const fragment = line.substring(start, pos)
            result += this.FormatFragment(fragment)
            start = pos
            pos = MatchPairingCharacter(line, start, char)
            result += line.substring(start, pos)
            start = pos
            pos = MatchPairingCharacter(line, start + 1)
        }
        if (start < line.length) {
            result += this.FormatFragment(line.substring(start))
        }
        if (end < 0) {
            return result
        } else {
            return result + annotation;
        }
    }
    private FormatFragment(fragment: string): string {
        fragment = fragment.replace(/(?<=[\)\]\}\'\"]|\b)\s*(?=[&|^<>=\+\-*/%!`?:])/gi, " ");
        fragment = fragment.replace(/(?<=[&|^<>=\+\-*/%?])\s*(?=[\(\[\{\'\"]|\b)/gi, " ");
        fragment = fragment.replace(/(?<=[:,;])\s*/gi, " ");

        fragment = fragment.replace(/(?<=[\)\]\}\'\"]|\b)\s*\?\s*(?=[\.\(\[\{]|(\->))/gi, "?");
        fragment = fragment.replace(/(?<=[\)\]\}\'\"?]|\b)\s*\->\s*(?=\b)/gi, "->");

        fragment = fragment.replace(/(?<=\b|\'|\")\s*(?=[,;\)\]\}\.\(\[\{]|\+\+|\-\-)/gi, "");
        fragment = fragment.replace(/(?<=[\(\[\{\.!`]|\+\+|\-\-)\s*\b/gi, "");
        fragment = fragment.replace(/(?<=[\)\]\}]|\+\+|\-\-)\s*(?=[\.\(\[\{])/gi, "");
        fragment = fragment.replace(/(?<=[\(\[\{])\s*(?=[&|^<>=\+\-*/%?])/gi, " ");
        fragment = fragment.replace(/(?<=[&|^<>=*/%?,;\(\[\{])\s*([\+\-])\s*(?=\b|\'|\")/gi, " $1");
        fragment = fragment.replace(/([&|^<>=\+\-*/%?])\s+([&|^<>=\+\-*/%?])\s*\b/gi, "$1 $2");
        return fragment;
    }
}