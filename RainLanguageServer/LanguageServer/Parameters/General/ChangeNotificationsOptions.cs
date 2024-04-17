using LanguageServer.Json;

namespace LanguageServer.Parameters.General
{
    /// <summary>
    /// For <c>initialize</c>
    /// </summary>
    /// <seealso>Spec 3.6.0</seealso>
    public class ChangeNotificationsOptions : Either
    {
        /// <summary>
        /// Defines an implicit conversion of a <see cref="string"/> to a <see cref="ChangeNotificationsOptions"/>
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public static implicit operator ChangeNotificationsOptions(string value) => new(value);

        /// <summary>
        /// Defines an implicit conversion of a <see cref="bool"/> to a <see cref="ChangeNotificationsOptions"/>
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public static implicit operator ChangeNotificationsOptions(bool value) => new(value);

        /// <summary>
        /// Initializes a new instance of <c>ChangeNotificationsOptions</c> with the specified value.
        /// </summary>
        /// <param name="value"></param>
        public ChangeNotificationsOptions(string value) : base(value, typeof(string)) { }

        /// <summary>
        /// Initializes a new instance of <c>ChangeNotificationsOptions</c> with the specified value.
        /// </summary>
        /// <param name="value"></param>
        public ChangeNotificationsOptions(bool value) : base(value, typeof(bool)) { }
    }
}
