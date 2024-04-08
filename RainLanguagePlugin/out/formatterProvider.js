"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const vscode_1 = require("vscode");
class FormatProvider {
    async provideDocumentRangeFormattingEdits(document, range, options, token) {
        let results = [];
        for (let index = range.start.line; index <= range.end.line; index++) {
            const line = document.lineAt(index);
            const newText = this.FormatLine(line.text);
            if (line.text != newText) {
                results.push(new vscode_1.TextEdit(line.range, newText));
            }
        }
        return results;
    }
    async provideOnTypeFormattingEdits(document, position, ch, options, token) {
        const line = document.lineAt(position.line - 1);
        const newText = this.FormatLine(line.text);
        if (line.text != newText) {
            return [
                new vscode_1.TextEdit(line.range, this.FormatLine(line.text))
            ];
        }
        else {
            return [];
        }
    }
    MatchPairingCharacter(line, start, char) {
        if (char == null) {
            let ss = line.indexOf("\'", start);
            let ds = line.indexOf("\"", start);
            if (ss < 0)
                return ds;
            else if (ds < 0)
                return ss;
            else if (ss < ds)
                return ss;
            else
                return ds;
        }
        else {
            let pos = line.indexOf(char, start);
            if (pos < 0)
                return line.length;
            if (pos > 0 && line[pos - 1] == '\\')
                return this.MatchPairingCharacter(line, pos + 1, char);
            return pos;
        }
    }
    FormatLine(line) {
        let result = "";
        let start = 0;
        let pos = this.MatchPairingCharacter(line, 0);
        while (pos > 0) {
            let char = line[pos];
            pos += 1;
            result += this.FormatFragment(line.substring(start, pos));
            start = pos;
            pos = this.MatchPairingCharacter(line, start, char);
            result += line.substring(start, pos);
            start = pos;
            pos = this.MatchPairingCharacter(line, start + 1);
        }
        if (start < line.length) {
            result += this.FormatFragment(line.substring(start));
        }
        return result;
    }
    FormatFragment(fragment) {
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
exports.default = FormatProvider;
//# sourceMappingURL=formatterProvider.js.map