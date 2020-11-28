#pragma once

namespace EEF
{
    class EquipEventHandler :
        public BSTEventSink <TESEquipEvent>

    {
    protected:
        virtual EventResult	ReceiveEvent(TESEquipEvent* evn, EventDispatcher<TESEquipEvent>* dispatcher) override;

    public:
        static EquipEventHandler* GetSingleton() {
            static EquipEventHandler handler;
            return &handler;
        }
    };

    bool Initialize();
}