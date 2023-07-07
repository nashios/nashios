# Commit style

The NashiOS commit style is a set of rules for writing commit messages. It provides a set of guidelines for creating a commit message that is easy to read and understand and make the commit history more useful.

The commit should be structured as follows:

```
<type> [scope]: <description>

[optional body]

[optional footer(s)]
```

## Types

The type is contained within the title and can be one of these types:

- **build**: Changes that affect the build system or external dependencies (example scopes: cmake, scripts).
- **ci**: Changes to our CI configuration files and scripts (example scopes: github).
- **docs**: Documentation only changes, used when editing readme files, documentation, etc.
- **feat**: A new feature was added to the project.
- **fix**: Patches a bug in the codebase.
- **perf**: A code change that improves performance.
- **refactor**: A code change that neither fixes a bug nor adds a feature.
- **style**: Changes that do not affect the meaning of the code (white-space, formatting, missing semi-colons, etc).
- **test**: Adding missing tests or correcting existing tests.
- **revert**: Reverts a previous commit.

## Scopes

The scope could be anything specifying the place of the commit change. 

For example `kernel`, `libc`, `system_service`, `desktop`, `shell`, etc.

## Examples

### Commit message with description and breaking change footer

```
refactor [kernel]: add new virtual filesystem implementation

BREAKING_CHANGE: The old virtual filesystem implementation was removed.
```

### Commit message with multi-paragrapgh body and multiple footers

```
fix [window_service]: prevent racing condition

The window service was not handling the window creation correctly, this
was causing a racing condition that could lead to a kernel panic.

Remove timeouts which were used to mitigate the racing condition.

Reviewed-by: @saullo
Refers-to: #123
```