﻿namespace LanguageServer.Parameters.TextDocument
{
    /// <summary>
    /// For <c>textDocument/completion</c> and <c>completionItem/resolve</c>
    /// </summary>
    /// <seealso>Spec 3.9.0</seealso>
    public class CompletionItem
    {
        /// <summary>
        /// The label of this completion item.
        /// </summary>
        public string? label;

        /// <summary>
        /// The kind of this completion item.
        /// </summary>
        public CompletionItemKind? kind;

        /// <summary>
        /// A human-readable string with additional information
        /// about this item, like type or symbol information.
        /// </summary>
        public string? detail;

        /// <summary>
        /// A human-readable string that represents a doc-comment.
        /// </summary>
        /// <seealso>Spec 3.3.0</seealso>
        public Documentation? documentation;

        /// <summary>
        /// Indicates if this item is deprecated.
        /// </summary>
        /// <seealso>Spec 3.7.2</seealso>
        public bool? deprecated;

        /// <summary>
        /// Select this item when showing.
        /// </summary>
        /// <remarks>
        /// Note that only one completion item can be selected and that the tool / client decides which item that is.
        /// The rule is that the <b>first</b> item of those that match best is selected.
        /// </remarks>
        /// <seealso>Spec 3.9.0</seealso>
        public bool? preselect;

        /// <summary>
        /// A string that should be used when comparing this item with other items.
        /// </summary>
        public string? sortText;

        /// <summary>
        /// A string that should be used when filtering a set of completion items.
        /// </summary>
        public string? filterText;

        /// <summary>
        /// A string that should be inserted into a document when selecting this completion.
        /// </summary>
        /// <seealso>Spec 3.3.0</seealso>
        [Obsolete("insertText is deprecated since the spec v3.3.0, please use textEdit instead.")]
        public string? insertText;

        /// <summary>
        /// The format of the insert text.
        /// </summary>
        public InsertTextFormat? insertTextFormat;

        /// <summary>
        /// An edit which is applied to a document when selecting this completion.
        /// </summary>
        public TextEdit? textEdit;

        /// <summary>
        /// An optional array of additional text edits that are applied when selecting this completion.
        /// </summary>
        public TextEdit[]? additionalTextEdits;

        /// <summary>
        /// An optional set of characters that when pressed while this completion is active
        /// will accept it first and then type that character.
        /// </summary>
        /// <remarks>
        /// Note that all commit characters should have <c>length=1</c> and that superfluous characters will be ignored.
        /// </remarks>
        /// <seealso>Spec 3.2.0</seealso>
        public string[]? commitCharacters;

        /// <summary>
        /// An optional command that is executed <b>after</b> inserting this completion.
        /// </summary>
        public Command? command;

        /// <summary>
        /// An data entry field that is preserved on a completion item between a completion and a completion resolve request.
        /// </summary>
        public dynamic? data;
    }
}
