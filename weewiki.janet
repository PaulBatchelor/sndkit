(def ww-dir "_site/sndkit")
(def webroot (if (ww-server?) "/wiki" "/sndkit"))
(def write-tex (if (ww-server?) false true))
(def fig-dir "_fig")

(defn pgexists? (name)
  (var db (ww-db))
  (var x
       (sqlite3/eval
        db (string
            "SELECT EXISTS(SELECT key from wiki "
            "where key is \""name"\") as doiexist;")))
  (= ((x 0) "doiexist") 1))

# (defn pglink (link)
#   (cond
#     (= link "index")
#     (string webroot "/")
#     (pgexists? link)
#     (string webroot "/" link) "#"))

(defn pglink (page &opt target)
  (var link "")
  (if (nil? target)
    (set link page)
    (set link (string page "#" target)))
  (cond
    (= page "index")
    (string webroot "/")
    (pgexists? page)
    (string webroot "/" link) "#"))


(defn refstr (link &opt name)
  (if (nil? name)
    (string "[[" (pglink link) "][" link "]]")
    (string
     "[["
     (pglink link)
     "]["
     name
     "]]")))

# (defn ref (link &opt name)
#   (if (nil? name)
#     (org (string "[[" (pglink link) "][" link "]]"))
#     (org
#      (string
#       "[["
#       (pglink link)
#       "]["
#       name
#       "]]"))))

(defn ref (link &opt name target)
  (default target nil)
  (if (nil? name)
    (org (string "[[" (pglink link) "][" link "]]"))
    (org
     (string
      "[["
      (pglink link target)
      "]["
      name
      "]]"))))

(defn img [path &opt alt srcset]
  (print
   (string
    "<img src=\""
    path "\""
    (if-not (nil? alt) (string " alt=\"" alt "\""))
    (if-not (nil? srcset)
      (string "srcset=\"" srcset "\""))
    ">")))

(defn img-link [path link &opt alt]
  (print
   (string
    "<a href=\"" link "\">"
    "<img src=\""
    path "\""
    (if-not (nil? alt) (string " alt=\"" alt "\""))
    "></a>")))

(defn html-header
  []
(print
``<!DOCTYPE html>
<html lang="en">
<head>

<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link rel="stylesheet" href="/css/style.css">

</head>
<body>
<div id="main">
``)
)

(defn html-footer
  []
  (print
``
</div>
</body>
</html>
``
))

(defn marker [id &opt msg]
  (default msg "")
  (prin (string "<a id=\"" id "\">" msg "</a>")))

(import "keywords")
(import "fig")


(defn pngfilepath [name]
  (if (ww-server?)
    (string "/" fig-dir "/" name ".png")
    (string webroot "/" fig-dir "/" name ".png")))

(defn fig (name eqn)
  (if write-tex (fig/fig name eqn fig-dir))
  (img
   (pngfilepath name)
   eqn
   (string
    (pngfilepath name) " 1x,"
    (pngfilepath (string name "2x")) " 2x,")
))

(defn smallfig (name &opt eqn)
  (if write-tex (fig/smallfig name eqn fig-dir))
  (img
   (pngfilepath name)
   eqn))
