# How to model VC action and detect likely merge conflicts

I'd like to study how choices that programmers make about code affect
the amount of time that will eventually be spent manually resolving
merge conflicts. That's undoubtedly a hard problem, so we'll start with
a model that's one step better than spherical cow level.

I'm building a statistical Monte Carlo, here. In each iteration, I'll
set some parameters intended to represent the effects of a particular
coding choice (short files versus long files, or always adding new code
at the end versus some scheme for interleaving it), run a bunch of
random trials, and collect statistics about how much work is required to
handle simulated merge conflicts.

## Defining an abstract project

First of all a project is a moving target. Otherwise merge conflicts
wouldn't be a thing. So to manage that evolution we'll define a "project
state" abstraction and a way to mutate a ancestor state into a descendant state.

The state of a project at any time will be defined as zero or more files
each containing zero or more code elements.

"Code elements" are our second big abstraction here. They might
represent a declaration, an expression, a statement a compound statement
like a loop, an important comment, or anything else. They might
represent code in one language or another, instructions to the build
system and or documentation maintained in the repository. But we don't
care about any of that: they're all just chunks of data that can be
created, changed, left alone, moved, or deleted.

## The models

A single iteration consists of setting up an ancestor state, generating two
descendant states which we then compare to determine how a merge between
them might go.

### Evolving a project state

The place to start is thinking about what evolutions can happen to real
projects, then select a set up changes in out model to represent them.
Real project evolutions can see

* Code edited in place
* Code added in an existing file
* Code in an existing file removed
* Code moved (with or without changes) in its original file
* Code moved (with or without changes) to a different file
* Files added
* Files removed

#### Simplified model of project evolution

We're going to support four kinds of changes (and try to make cleverness
fill in the gaps):

* Edit in place: just mark the code element as changed.
* Add code: randomly choose the destination file, including an as yet
  non-existent destination (i.e. creating a new file).
* Move code, randomly choose the destination, including new file
  creation as above.
* Remove code element.

And file creation and deletion are organic: files are created when they
first obtain an element and removed if they drop to zero elements.

#### Frequency of application

The hard work is going to be selecting the rates for each operation and
the random selection procedure so that they are at least vaguely like
those in real projects.

I suspect that edit-in-place and add-code are relatively common, while
delete-code is a bit less common. Move code is a weird beast: the
extract-function refactoring mechanism is exactly that and I use it
pretty often, but mostly on code I've added but not yet checked in.

### Detecting conflicts

Maybe defer these decisions to git.mergetool?

The auto-merge algorithm will not be away of cross-file moves: the
elements will be "deleted" in the source file and "added" in the
destination.

In detecting conflict we have three project states to refer to: the
common ancestor, and two evolved states. We're going to walk
simultaneously only the element list in each of the states, largely
using the ancestor as ground-truth. 

We can automatically handle

* Both branches have the same ID and no more than one is changed
* One branch tracks the ancestor, and the other has new code (added)
* Original deleted in either or both branches.

We treat a conflict when

* The original is edited in both branches
* Different additions are present in both branches (??)


### Estimating the effort related to a conflict

The key for me is that if there is a conflict I get to manually handle
all the changes, even the ones that are not in conflict. On the other
hand, those ones that auto-merge could have handled are usually faster
and easier in the merge-tool: I don't have to do any research to see
the right solution.

The way I'm going to handle this at first is by reporting out

* The total number of automatically handled changes
* The total number of changes that _could_ have been automatically
  handled but were none the less referred to the human.
* The total number of conflicting changes 
