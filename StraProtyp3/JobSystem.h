#pragma once


#include "ViennaJobSystem/VGJS.h"


class JobSystem {
public:
    static JobSystem* get() {

        if (!g_pJobSystem) g_pJobSystem = new JobSystem();

        return g_pJobSystem;
    }

    static void del() {
        if (g_pJobSystem) {

            vgjs::terminate();
            vgjs::wait_for_termination();

            delete g_pJobSystem;

        }
    }


    vgjs::JobSystem* getVgjsJobSystem() const { return p_vgjsJobSystem; }

private:
    static JobSystem* g_pJobSystem;

    vgjs::JobSystem* p_vgjsJobSystem = nullptr;

private:
    JobSystem();
    ~JobSystem();

};
