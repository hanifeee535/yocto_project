# COMP.CE.460 Embedded Linux Drivers

--------------------------------------------------------------------------------
## Quick guide to resume your work after rebooting the VM

If you are using HTTPS remotes, you might want to run `git config --global credential.helper 'cache --timeout=3600'` before cloning: this way you will have to input your username and password only after 1 hour without interacting with your remote.

1. `cd ~`
2. `git clone --recurse-submodules <STUDENT_REPO_URL> [<LOCAL_CLONE_DIR>]`
   - by default, if you don't specify `<LOCAL_CLONE_DIR>` the local clone will be in `~/NN/`, if for any reason, during the first exercise you used a different name or path, make sure to replicate the one you used previously, or the layers configuration you committed previously will be incorrect;

## Quick guide to pull new exercise instructions

It is sufficient to run the following script (**from the root of your repository**):

```bash
cd <STUDENT_REPO_ROOT>
scripts/update_from_course_upstream.sh
```

Alternatively you could [download the script from your browser][update_from_course_upstream] to `/tmp/`, and then run it **from the root of your repository**:

```bash
cd <STUDENT_REPO_ROOT>
bash /tmp/update_from_course_upstream.sh
```

## Quick guide to submit the exercise assignment

1. `git commit` - remember to *add* new files
2. `git push`
3. Create tag for submission in GitLab - format `ExNN_submissionN`
4. `git push --tags`
5. Submit a link to the tag on the corresponding assignment in [Moodle][moodle.COMP.CE.460]

## How to use SSH remotes

Using HTTPS remotes becomes quickly frustrating as it requires to input your credentials several times, even multiple times for a single `git` command.

Read [Using a persistent SSH key for project access][semipermanent-ssh-keys] for tips on how to securely use a semipermanent `git` profile with SSH keys for the shared VM environment of the course.

**Note**: If you choose to setup access through SSH keys, remember to use the SSH remote URL to clone your student repository and for the `course_upstream` remote.

If you decide to use HTTPS remotes, you might want to run `git config --global credential.helper 'cache --timeout=3600'` before cloning: this way you will have to input your username and password only after 1 hour without interacting with your remote.

## Git Help

If you need training to use Git, visit [Git Course in Plussa][git-plussa].


[moodle.COMP.CE.460]: https://moodle.tuni.fi/course/view.php?id=44546
[git-plussa]: https://plus.tuni.fi/tie-git/
[course_upstream project]: https://course-gitlab.tuni.fi/comp-ce-460-embedded-linux-drivers_2024-2025/course_upstream
[semipermanent-ssh-keys]: semipermanent-ssh-keys.md
[update_from_course_upstream]: https://course-gitlab.tuni.fi/comp-ce-460-embedded-linux-drivers_2024-2025/course_upstream/raw/main/scripts/update_from_course_upstream.sh