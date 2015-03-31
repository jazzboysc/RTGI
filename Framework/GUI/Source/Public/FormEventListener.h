#pragma once

using namespace System;

namespace RTGI {
    namespace GUIFramework {

        class FormEventListener
        {
        public:
            FormEventListener();
            virtual ~FormEventListener();

            virtual void OnButtonClick(System::Object^  sender, System::EventArgs^  e);
            virtual void OnRadioButtonClick(System::Object^  sender, System::EventArgs^  e);
            virtual void OnCheckBoxClick(System::Object^  sender, System::EventArgs^  e);
        };
    }
}