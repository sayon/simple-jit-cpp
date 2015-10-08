#include "minimizer.h"
#include "../../translator/ssa_utils.h"

namespace mathvm {
    namespace IR {

        void Minimizer::replaceSucc(Block * parent, Block * old) {
            Block *subst = (Block *) old->getTransition()->asJumpAlways()->destination;

            if (parent->getTransition()->isJumpAlways()) {
                JumpAlways const *jmp = parent->getTransition()->asJumpAlways();
                assert(jmp->destination == old);
                parent->link(subst);
            }
            else {
                JumpCond const *jmp = parent->getTransition()->asJumpCond();
                if (jmp->yes == old) parent->setTransition(jmp->replaceYes(subst));
                if (jmp->no == old) parent->setTransition(jmp->replaceNo(subst));
                subst->addPredecessor(parent);
            }

            subst->removePredecessor(old);
        }

        IrElement *Minimizer::visit(const Function *const expr) {
            Function *fr = (Function *) base::visit(expr);

            for (auto b : blocksOrder(fr->entry)) {
                _debug << "Processing " << b->name << std::endl;
                if (b->isEmpty() && !b->isEntry() && !b->isLastBlock() && b->getTransition()->isJumpAlways()) {
                    _debug << "removing empty block " << b->name << std::endl;
                    removeEmpty((Block*)b);
//                    auto preds = b->predecessors;
//                    for (auto p : preds)
//                        replaceSucc((Block *) p, (Block *) b);

                }
                else _debug << "block " << b->name << " is not empty" << std::endl;
            }
            return fr;
        }

        void Minimizer::operator()() {
            Transformation::visit(&_oldIr);
        }

        void Minimizer::removeEmpty(Block *b) {
            auto parents = b->predecessors;
            assert (!b->isLastBlock());
            auto subst = b->getTransition()->asJumpAlways()->destination;
            for (auto p: parents) ((Block*)p)->relink(b, subst);
        }
    }

}