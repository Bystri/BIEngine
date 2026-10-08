#pragma once

#include "CharacterInputHistory.h"

namespace BIEngine {
class Actor;
}

class CharacterMovementReconciler {
public:
   static bool Reconcile(BIEngine::Actor& actor,
                         const CharacterMovementSnapshot& snapshot,
                         CharacterInputHistory& history,
                         float fixedDt);
};
