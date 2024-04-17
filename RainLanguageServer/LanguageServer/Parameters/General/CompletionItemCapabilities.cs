﻿namespace LanguageServer.Parameters.General
{
    /// <summary>
    /// For <c>initialize</c>
    /// </summary>
    /// <seealso>Spec 3.9.0</seealso>
    public class CompletionItemCapabilities
    {
        /// <summary>
        /// Client supports snippets as insert text.
        /// </summary>
        /// <remarks>
        /// A snippet can define tab stops and placeholders with <c>$1</c>, <c>$2</c>
        /// and <c>${3:foo}</c>. <c>$0</c> defines the final tab stop, it defaults to
        /// the end of the snippet. Placeholders with equal identifiers are linked,
        /// that is typing in one will update others too.
        /// </remarks>
        public bool? snippetSupport;

        /// <summary>
        /// Client supports commit characters on a completion item.
        /// </summary>
        /// <seealso>Spec 3.2.0</seealso>
        public bool? commitCharactersSupport;

        /// <summary>
        /// Client supports the follow content formats for the documentation
        /// property.The order describes the preferred format of the client.
        /// </summary>
        /// <value>
        /// See <see cref="LanguageServer.Parameters.MarkupKind"/> for an enumeration of standardized kinds.
        /// </value>
        /// <seealso>Spec 3.3.0</seealso>
        /// <seealso cref="LanguageServer.Parameters.MarkupKind"/>
        public string[]? documentationFormat;

        /// <summary>
        /// Client supports the deprecated property on a completion item.
        /// </summary>
        /// <seealso>Spec 3.7.2</seealso>
        public bool? deprecatedSupport;

        /// <summary>
        /// Client supports the preselect property on a completion item.
        /// </summary>
        /// <seealso>Spec 3.9.0</seealso>
        public bool? preselectSupport;
    }
}
