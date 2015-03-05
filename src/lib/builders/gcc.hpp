#include "native_compilable.hpp"

namespace bacs{namespace system{namespace builders
{
    class gcc: public native_compilable
    {
    public:
        explicit gcc(const std::vector<std::string> &arguments);

    protected:
        ProcessPointer create_process(
            const ProcessGroupPointer &process_group,
            const name_type &name) override;

    private:
        std::string m_executable;
        std::vector<std::string> m_flags;
    };
}}}
