#ifndef __MODULE_MODULETESTA_HPP__
#define __MODULE_MODULETESTA_HPP__

#include <Helena/Common/HFModule.hpp>

namespace Helena
{
    // Declaration
    class ModuleTestB;

    // Module class
    class ModuleTestA final : public HFModule
    {
    public:
        explicit ModuleTestA() : m_pModuleTestB(nullptr) {}
        ~ModuleTestA() override = default;

    protected:
        bool AppInit()      override;
        bool AppConfig()    override;
        bool AppStart()     override;
        bool AppUpdate()    override;
        bool AppShut()      override;

    private:
        ModuleTestB* m_pModuleTestB;
    };
}
#endif // __MODULE_MODULETESTA_HPP__