//https://github.com/fraillt/bitsery/issues/29


#pragma once


#include <bitsery/details/serialization_common.h>
#include <bitsery/details/adapter_common.h>
namespace bitsery {
    namespace ext {

        template <typename SerFnc, typename DesFnc>
        class SaveAndLoad {
        public:
            SaveAndLoad(SerFnc serFnc, DesFnc desFnc)
                :_serFnc{ std::move(serFnc) },
                _desFnc{ std::move(desFnc) } {}

            template<typename Ser, typename T, typename Fnc>
            void serialize(Ser& ser, const T& obj, Fnc&&) const {
                _serFnc(ser, obj);
            }

            template<typename Des, typename T, typename Fnc>
            void deserialize(Des& des, T& obj, Fnc&&) const {
                _desFnc(des, obj);
            }
        private:
            std::decay_t<SerFnc> _serFnc;
            std::decay_t<DesFnc> _desFnc;
        };
    }

    namespace traits {
        template<typename T, typename SerFnc, typename DesFnc>
        struct ExtensionTraits<ext::SaveAndLoad<SerFnc, DesFnc>, T> {
            using TValue = void;
            static constexpr bool SupportValueOverload = false;
            static constexpr bool SupportObjectOverload = true;
            static constexpr bool SupportLambdaOverload = false;
        };

    }

}