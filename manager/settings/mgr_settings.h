//
// Created by RGAA on 26/02/2025.
//

#ifndef GAMMARAYSERVER_RELAY_SETTINGS_H
#define GAMMARAYSERVER_RELAY_SETTINGS_H

namespace tc
{
    class MgrSettings {
    public:
        static MgrSettings* Instance() {
            static MgrSettings instance;
            return &instance;
        }

        void LoadSettings();
        void DumpSettings();

    public:
        int listen_port_ = 0;

    };
}

#endif //GAMMARAYSERVER_RELAY_SETTINGS_H
