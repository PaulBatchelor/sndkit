# open and clear wiki db

(ww-open "a.db")
(ww-clear)

# unlinked pages


# linked pages

(ww-add-link "index" "index.org")
(ww-add-link "bigverb" "bigverb.org")
(ww-add-link "osc" "osc.org")
(ww-add-link "dcblocker" "dcblocker.org")
(ww-add-link "fmpair" "fmpair.org")
(ww-add-link "vardelay" "vardelay.org")
(ww-add-link "rline" "rline.org")
(ww-add-link "peakeq" "peakeq.org")
(ww-add-link "modalres" "modalres.org")
(ww-add-link "phasewarp" "phasewarp.org")

# sync and close

(ww-sync)
(ww-close)
