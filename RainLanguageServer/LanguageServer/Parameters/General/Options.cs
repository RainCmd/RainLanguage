﻿using System;
using System.Collections.Generic;
using System.Text;

namespace LanguageServer.Parameters.General
{
    /// <summary>
    /// For <c>initialize</c>
    /// </summary>
    /// <remarks>
    /// a detailed structure defining each notification.
    /// </remarks>
    public class TextDocumentSyncOptions
    {
        /// <summary>
        /// Open and close notifications are sent to the server.
        /// </summary>
        public bool? openClose;
        /// <summary>
        /// Change notifications are sent to the server.
        /// </summary>
        public TextDocumentSyncKind? change;
        /// <summary>
        /// Will save notifications are sent to the server.
        /// </summary>
        public bool? willSave;
        /// <summary>
        /// Will save wait until requests are sent to the server.
        /// </summary>
        public bool? willSaveWaitUntil;
        /// <summary>
        /// Save notifications are sent to the server.
        /// </summary>
        public SaveOptions? save;
    }

    /// <summary>
    /// For <c>initialize</c>
    /// </summary>
    /// <remarks>
    /// Save options.
    /// </remarks>
    public class SaveOptions
    {
        /// <summary>
        /// The client is supposed to include the content on save.
        /// </summary>
        public bool? includeText;
    }

    /// <summary>
    /// For <c>initialize</c>
    /// </summary>
    /// <remarks>
    /// Completion options.
    /// </remarks>
    public class CompletionOptions
    {
        /// <summary>
        /// The server provides support to resolve additional information for a completion item.
        /// </summary>
        public bool? resolveProvider;

        /// <summary>
        /// The characters that trigger completion automatically.
        /// </summary>
        public string[]? triggerCharacters;
    }

    /// <summary>
    /// For <c>initialize</c>
    /// </summary>
    /// <remarks>
    /// Signature help options.
    /// </remarks>
    public class SignatureHelpOptions
    {
        /// <summary>
        /// The characters that trigger signature help automatically.
        /// </summary>
        public string[]? triggerCharacters;
    }

    /// <summary>
    /// For <c>initialize</c>
    /// </summary>
    /// <remarks>
    /// Code Lens options.
    /// </remarks>
    public class CodeLensOptions
    {
        /// <summary>
        /// Code lens has a resolve provider as well.
        /// </summary>
        public bool? resolveProvider;
    }

    /// <summary>
    /// For <c>initialize</c>
    /// </summary>
    /// <remarks>
    /// Format document on type options.
    /// </remarks>
    public class DocumentOnTypeFormattingOptions
    {
        /// <summary>
        /// A character on which formatting should be triggered, like <c>}</c>.
        /// </summary>
        public string? firstTriggerCharacter;

        /// <summary>
        /// More trigger characters.
        /// </summary>
        public string[]? moreTriggerCharacter;
    }

    /// <summary>
    /// For <c>initialize</c>
    /// </summary>
    /// <remarks>
    /// Document link options.
    /// </remarks>
    public class DocumentLinkOptions
    {
        /// <summary>
        /// Document links have a resolve provider as well.
        /// </summary>
        public bool? resolveProvider;
    }

    /// <summary>
    /// For <c>initialize</c>
    /// </summary>
    /// <remarks>
    /// Execute command options.
    /// </remarks>
    public class ExecuteCommandOptions
    {
        /// <summary>
        /// The commands to be executed on the server
        /// </summary>
        public string[]? commands;
    }
}
