(defn fig [name eqn &opt dir]
  (default dir ".")
  (var fp (file/open (string dir "/" name ".tex") :w))
  (file/write
   fp
   (string
    "\\hsize 0pt\n"
    "\\vsize 0pt\n"
    "\\nopagenumbers\n"
    "\\overfullrule 0pt\n"
    "\\noindent\n"
    "$$\n"))
  (file/write fp eqn)
  (file/write fp "\n$$\n\\bye")
  (file/close fp))

(defn smallfig [name &opt eqn dir]
  (default dir ".")
  (default eqn nil)
  (if-not (nil? eqn)
    (do
      (var fp (file/open (string dir "/" name ".tex") :w))
      (file/write
       fp
       (string
        "\\hsize 0pt\n"
        "\\vsize 0pt\n"
        "\\nopagenumbers\n"
        "\\overfullrule 0pt\n"
        "\\noindent\n"
        "$\n"))
      (file/write fp eqn)
      (file/write fp "\n$\n\\bye")
      (file/close fp))))
