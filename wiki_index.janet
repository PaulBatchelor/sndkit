(defn mkpage ()
  #(var db (sqlite3/open "a.db"))
  (var db (ww-db))
  (each page
      (sqlite3/eval
       db
       (string "SELECT key FROM wiki "
               "WHERE key NOT LIKE \"@%\" "
               "ORDER BY key COLLATE NOCASE;"))
    (do
      # handle index page differently
      (if (= (get page "key") "index")
        (org (string "[["
                     (string webroot "/index.html")
                     "][index]]"))
        (ref (get page "key") (get page "key")))
      (org "\n")))
   #(sqlite3/close db)
)
