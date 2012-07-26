So this is a bit of a mess.

The original repository is at <a href="http://code.google.com/p/sooty">Google Code</a>,
and it turns out I suck at importing from SVN. Some glaring flaws in the original design
of Sooty's tree construction were revealed through more advanced usage, and required some
pretty fundamental changes to fix. Those went into a branch, which ended up being
used as the basis for this git repository you see now.

Which is to say that the history was lost, some things may be out of place, and some
things have not been rewritten to work with the new plumbing. This doesn't actually bother
Sooty, however current versions of Prism are just going to fall over.