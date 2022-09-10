#include "Process.h"

namespace BIEngine
{

	Process::Process()
		: m_state(State::UNINITIALIZED)
	{

	}

	Process::~Process() {
		if (m_pChild) {
			m_pChild->OnAbort();
		}
	}

	StrongProcessPtr Process::RemoveChild() {
		if (m_pChild) {
			StrongProcessPtr pChild = m_pChild;
			m_pChild.reset();

			return pChild;
		}

		return StrongProcessPtr();
	}

}