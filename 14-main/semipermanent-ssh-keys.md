# Introduction

Accessing remote git repositories using HTTPS quickly becomes inconvenient: depending on the configuration of a git repository, a single `git pull` might require to input the username/password three or more times.

Usually, generating a SSH keypair and adding the public half to the remote user profile would be easy and preferred, but working on a shared VM which erases its contents at every reboot creates some challenges: the goal of this document is to illustrate a potential workflow to securely use SSH git remotes in this environment.

## Table of Contents

1. [Workflow to generate a semipermanent git profile](#workflow-to-generate-a-semipermanent-git-profile)
2. [Workflow to restore your profile after VM reboot](#workflow-to-restore-your-profile-after-vm-reboot)
3. [Update existing `git` remotes to use an SSH URL](#update-existing-git-remotes-to-use-an-ssh-url)
4. [Alternatives](#alternatives)




# Workflow to generate a semipermanent git profile

These instructions should be followed only once, when generating for the first time your profile and keys.
The newly generated private key and the profile archive will be encrypted using (different) passphrases to minimize security risks associated with the shared environment.

## Generate SSH public keypair

- `ssh-keygen -t ed25519`
- default location is recommended
- **NOTICE**: Set a (strong) passphrase on the SSH private key (the `ssh-agent` will automatically ensure you will not have to enter this passphrase several times in a session)
- add the new public key to your Gitlab profile
  - `cat $HOME/.ssh/id_ed25519.pub` to show the public key
  - on Gitlab web UI: `User > Settings > SSH Keys`

## Add the new public key to your Gitlab profile

## Create a global `git` configuration

~~~bash
git config --global user.name "Mario Super"
git config --global user.email "the.mario@student.tut.fi"
~~~

## Create an encrypted tarball containing the profile

~~~bash
cd $HOME
tar cpvz .ssh .gitconfig | gpg --symmetric --output gitprofile.tar.gz.gpg
~~~

- Use a strong passphrase
- Avoid reusing the same passphrase protecting the SSH private key
- <u>Don't forget your passphrases!!</u>
- Save the encrypted archive either to an external USB drive (connecting it to the VM) or to your TUT profile folder (enabling and setting a Shared Folder from `VM > Settings > Options`)







# Workflow to restore your profile after VM reboot

- recover the encrypted tarball from the storage you selected (external USB drive or TUT profile folder)
- decrypt and restore the archive:
  ~~~bash
  gpg --decrypt /path/to/gitprofile.tar.gz.gpg | tar xpvz -C $HOME
  ~~~



# Update existing `git` remotes to use an SSH URL

If you used HTTPS URLs to define the remotes in your local `git` repository and wish to switch to SSH to avoid repeated user/password accesses, you can use the following commands:

~~~bash
cd /path/to/your/local/repository
git remote set-url <remote_name> <remote_SSH_URL>
~~~

Examples:

- `course_upstream`:
  ~~~bash
  git remote set-url course_upstream git@course-gitlab.tuni.fi:comp.ce.460-real-time-systems_2023-2024/course_upstream.git
  ~~~
- `origin` (student project, where `${GROUP_ID}` is your group number):
  ~~~bash
  GROUP_ID="<NN>"
  git remote set-url origin git@course-gitlab.tuni.fi:comp.ce.460-real-time-systems_2023-2024/${GROUP_ID}.git
  unset GROUP_ID
  ~~~


# Alternatives

The described workflow is by no mean the only one.
It would be perfectly fine, for example, to generate a new keypair for every session and add the ephemeral public key to Gitlab every time (and remove old ephemeral keys). In this case you would also need to set the `user.name` and `user.email` properties in the `git` configuration at every reboot.
