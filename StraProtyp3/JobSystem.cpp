#include "JobSystem.h"

JobSystem* JobSystem::g_pJobSystem = nullptr;

JobSystem::JobSystem() {

	p_vgjsJobSystem = new vgjs::JobSystem();

	// Setting main thread as index 0.
	// Thus we can make sure that all graphics stuff is run on main thread.
	//p_vgjsJobSystem->thread_task(vgjs::thread_index_t(0));
}

JobSystem::~JobSystem() {

}
