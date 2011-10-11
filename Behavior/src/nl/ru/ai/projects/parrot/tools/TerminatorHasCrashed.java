package nl.ru.ai.projects.parrot.tools;



/**
 *
 * @author sjoerdlagarde
 */
public class TerminatorHasCrashed extends TerminalCondition {

    @Override
    public boolean terminate() {
        return false;
    }

}
