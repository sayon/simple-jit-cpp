#include "ir.h"
#include "../translator/ssa_utils.h"
#include "transformations/identity.h"


namespace mathvm {
    namespace IR {

        Function::~Function() {
            if (entry)
                for( auto b : blocksPostOrder(entry))
                    delete b;
        }

        JumpCond* JumpCond::replaceYes(Block *const repl) const { return new JumpCond( repl, no,  (Atom const*) condition->visit(&copier)); }
        JumpCond* JumpCond::replaceNo(Block *const repl) const { return new JumpCond( yes, repl,  (Atom const*) condition->visit(&copier)); }

        bool Block::relink(Block * oldChild, Block* newChild) {

        if (isLastBlock()) return false;
            if (getTransition()->isJumpAlways() && getTransition()->asJumpAlways()->destination == oldChild) {
                link(newChild);
                oldChild->predecessors.erase(this);
                return true;
            }
            else {
                auto jc = getTransition()->asJumpCond();
                if (jc->yes == oldChild) link(jc->replaceYes(newChild));
                else if (jc->no == oldChild) link(jc->replaceNo(newChild));
                else return false;
                oldChild->predecessors.erase(this);
                return true;
            }
            return false;
        }
    }
}