namespace LanguageServer.Parameters.General
{
    /// <summary>
    /// For <c>initialize</c>
    /// </summary>
    public class SynchronizationCapabilities : RegistrationCapabilities
    {
        /// <summary>
        /// The client supports sending will save notifications.
        /// </summary>
        public bool? willSave;

        /// <summary>
        /// The client supports sending a will save request and
        /// waits for a response providing text edits which will
        /// be applied to the document before it is saved.
        /// </summary>
        public bool? willSaveWaitUntil;

        /// <summary>
        /// The client supports did save notifications.
        /// </summary>
        public bool? didSave;
    }
}
