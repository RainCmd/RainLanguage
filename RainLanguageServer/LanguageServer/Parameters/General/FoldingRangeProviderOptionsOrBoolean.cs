using LanguageServer.Json;

namespace LanguageServer.Parameters.General
{
    /// <summary>
    /// For <c>initialize</c>
    /// </summary>
    /// <remarks>
    /// The original spec describes the union type of
    /// <c>boolean</c>, <c>FoldingRangeProviderOptions</c>, or the intersection type
    /// <c>(FoldingRangeProviderOptions &amp; TextDocumentRegistrationOptions &amp; StaticRegistrationOptions)</c>.
    /// This implementation defines <see cref="FoldingRangeProviderOptions"/> class instead of the intersection type.
    /// </remarks>
    /// <seealso>Spec 3.10.0</seealso>
    public class FoldingRangeProviderOptionsOrBoolean : Either
    {
        /// <summary>
        /// Defines an implicit conversion of a <see cref="FoldingRangeProviderOptions"/> to a <see cref="FoldingRangeProviderOptionsOrBoolean"/>
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public static implicit operator FoldingRangeProviderOptionsOrBoolean(FoldingRangeProviderOptions value) => new(value);

        /// <summary>
        /// Defines an implicit conversion of a <see cref="bool"/> to a <see cref="FoldingRangeProviderOptionsOrBoolean"/>
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public static implicit operator FoldingRangeProviderOptionsOrBoolean(bool value) => new(value);

        /// <summary>
        /// Initializes a new instance of <c>FoldingRangeProviderOptionsOrBoolean</c> with the specified value.
        /// </summary>
        /// <param name="value"></param>
        public FoldingRangeProviderOptionsOrBoolean(FoldingRangeProviderOptions value) : base(value, typeof(FoldingRangeProviderOptions)) { }

        /// <summary>
        /// Initializes a new instance of <c>FoldingRangeProviderOptionsOrBoolean</c> with the specified value.
        /// </summary>
        /// <param name="value"></param>
        public FoldingRangeProviderOptionsOrBoolean(bool value) : base(value, typeof(bool)) { }

        /// <summary>
        /// Returns true if its underlying value is a <see cref="FoldingRangeProviderOptions"/>.
        /// </summary>
        public bool IsFoldingRangeProviderOptions => Type == typeof(FoldingRangeProviderOptions);

        /// <summary>
        /// Returns true if its underlying value is a <see cref="bool"/>.
        /// </summary>
        public bool IsBoolean => Type == typeof(bool);

        /// <summary>
        /// Gets the value of the current object if its underlying value is a <see cref="FoldingRangeProviderOptions"/>.
        /// </summary>
        public FoldingRangeProviderOptions FoldingRangeProviderOptions => GetValue<FoldingRangeProviderOptions>();

        /// <summary>
        /// Gets the value of the current object if its underlying value is a <see cref="bool"/>.
        /// </summary>
        public bool Boolean => GetValue<bool>();
    }
}
