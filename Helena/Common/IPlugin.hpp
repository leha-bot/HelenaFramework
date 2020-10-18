#ifndef COMMON_IPLUGIN_HPP
#define COMMON_IPLUGIN_HPP

namespace Helena
{
    class Service;
    class IPlugin
    {
        // Friend is used to get a clean interface without Set methods
        friend class ModuleManager;
        
    public:
        IPlugin() : m_Service(nullptr) {}
        virtual ~IPlugin() = default;
        IPlugin(const IPlugin&) = delete;
        IPlugin(IPlugin&&) = delete;
        IPlugin& operator=(const IPlugin&) = delete;
        IPlugin& operator=(IPlugin&&) = delete;

        virtual bool Initialize() {
            return true;
        }

        virtual bool Config() {
            return true;
        }

        virtual bool Execute() {
            return true;
        }

        virtual bool Update() {
            return true;
        }

        virtual bool Finalize() {
            return true;
        }

    public:
        [[nodiscard]] Service* GetService() const noexcept {
            return m_Service;
        }

    private:
        Service* m_Service;
    };
}

#endif // COMMON_IPLUGIN_HPP