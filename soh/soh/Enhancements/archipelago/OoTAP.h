class Archipelago {
    public:
        static Archipelago* Instance;
        bool isEnabled;
        bool isConnected;
        void Enable(const char* ip, int port, const char* slotName, const char* password);
        void Disable();
};