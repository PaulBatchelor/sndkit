(def ww-dir "_site/sndkit")
(def webroot "/sndkit")

(defn pgexists? (name)
  (var db (ww-db))
  (var x
       (sqlite3/eval
        db (string
            "SELECT EXISTS(SELECT key from wiki "
            "where key is \""name"\") as doiexist;")))
  (= ((x 0) "doiexist") 1))

(defn pglink (link)
  (cond
    (= link "index")
    (string webroot "/")
    (pgexists? link)
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

(defn ref (link &opt name)
  (if (nil? name)
    (org (string "[[" (pglink link) "][" link "]]"))
    (org
     (string
      "[["
      (pglink link)
      "]["
      name
      "]]"))))

(defn img [path &opt alt]
  (print
   (string
    "<img src=\""
    webroot "/_img/"
    path "\""
    (if-not (nil? alt) (string " alt=\"" alt "\""))
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
