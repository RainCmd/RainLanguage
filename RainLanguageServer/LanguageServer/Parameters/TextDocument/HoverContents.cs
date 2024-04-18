﻿using LanguageServer.Json;

namespace LanguageServer.Parameters.TextDocument
{
    /// <summary>
    /// The hover's content
    /// </summary>
    /// <remarks>
    /// <see cref="MarkedString"/> is deprecated, please use <see cref="MarkupContent"/> instead.
    /// </remarks>
    /// <seealso>Spec 3.3.0</seealso>
    public class HoverContents : Either
    {
        /// <summary>
        /// Defines an implicit conversion of a <see cref="string"/> to a <see cref="HoverContents"/>
        /// </summary>
        /// <param name="value"></param>
        public static implicit operator HoverContents(string value) => new(value);

        /// <summary>
        /// Defines an implicit conversion of a <see cref="T:System.String[]"/> to a <see cref="HoverContents"/>
        /// </summary>
        /// <param name="value"></param>
        public static implicit operator HoverContents(string[] value) => new(value);

        /// <summary>
        /// Defines an implicit conversion of a <see cref="MarkupContent"/> to a <see cref="HoverContents"/>
        /// </summary>
        /// <param name="value"></param>
        /// <seealso>Spec 3.3.0</seealso>
        public static implicit operator HoverContents(MarkupContent value) => new(value);

        /// <summary>
        /// Defines an implicit conversion of a <see cref="MarkupContent[]"/> to a <see cref="HoverContents"/>
        /// </summary>
        /// <param name="value"></param>
        /// <seealso>Spec 3.3.0</seealso>
        public static implicit operator HoverContents(MarkupContent[] value) => new(value);

        /// <summary>
        /// Initializes a new instance of <c>HoverContents</c> with the specified value.
        /// </summary>
        public HoverContents(string value) : base(value, typeof(string)) { }

        /// <summary>
        /// Initializes a new instance of <c>HoverContents</c> with the specified value.
        /// </summary>
        public HoverContents(string[] value) : base(value, typeof(string[])) { }

        /// <summary>
        /// Initializes a new instance of <c>HoverContents</c> with the specified value.
        /// </summary>
        /// <seealso>Spec 3.3.0</seealso>
        public HoverContents(MarkupContent value) : base(value, typeof(MarkupContent)) { }

        /// <summary>
        /// Initializes a new instance of <c>HoverContents</c> with the specified value.
        /// </summary>
        /// <seealso>Spec 3.3.0</seealso>
        public HoverContents(MarkupContent[] value) : base(value, typeof(MarkupContent[])) { }

        /// <summary>
        /// Returns true if its underlying value is a <see cref="string"/>.
        /// </summary>
        public bool IsString => Type == typeof(string);

        /// <summary>
        /// Returns true if its underlying value is a <see cref="T:System.String[]"/>.
        /// </summary>
        public bool IsStringArray => Type == typeof(string[]);

        /// <summary>
        /// Returns true if its underlying value is a <see cref="MarkupContent"/>.
        /// </summary>
        /// <seealso>Spec 3.3.0</seealso>
        public bool IsMarkupContent => Type == typeof(MarkupContent);
        public bool IsMarkupContentArray => Type == typeof(MarkupContent[]);

        /// <summary>
        /// Gets the value of the current object if its underlying value is a <see cref="string"/>.
        /// </summary>
        public string String => GetValue<string>();

        /// <summary>
        /// Gets the value of the current object if its underlying value is a <see cref="T:System.String[]"/>.
        /// </summary>
        public string[] StringArray => GetValue<string[]>();

        /// <summary>
        /// Gets the value of the current object if its underlying value is a <see cref="MarkupContent"/>.
        /// </summary>
        /// <seealso>Spec 3.3.0</seealso>
        public MarkupContent MarkupContent => GetValue<MarkupContent>();

        /// <summary>
        /// Gets the value of the current object if its underlying value is a <see cref="MarkupContent[]"/>.
        /// </summary>
        /// <seealso>Spec 3.3.0</seealso>
        public MarkupContent[] MarkupContentArray => GetValue<MarkupContent[]>();
    }
}
