//
// Created by RGAA on 26/02/2025.
//

#ifndef GAMMARAYSERVER_RELAY_SETTINGS_H
#define GAMMARAYSERVER_RELAY_SETTINGS_H

namespace tc
{
    class RelaySettings {
    public:
        static RelaySettings* Instance() {
            static RelaySettings instance;
            return &instance;
        }

        void LoadSettings();
        void DumpSettings();

    public:
        int listen_port_ = 0;

    };
}

#endif //GAMMARAYSERVER_RELAY_SETTINGS_H
