#include "native_compilable.hpp"

namespace bacs{namespace system{namespace builders
{
    class fpc: public native_compilable
    {
    public:
        explicit fpc(const std::vector<std::string> &arguments);

    protected:
        ProcessPointer create_process(
            const ProcessGroupPointer &process_group,
            const name_type &name) override;

    private:
        std::vector<std::string> m_flags;
    };
}}}
