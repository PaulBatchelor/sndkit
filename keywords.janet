(defn mkpage []
  (var db (sqlite3/open "keys.db"))
  (var keys
       (sqlite3/eval
        db
        (string
         "SELECT DISTINCT(keyword) from keywords "
         "WHERE keyword is NOT '' "
         "ORDER by keyword ASC;")))

  (each k keys
    (var pages
         (sqlite3/eval
          db
          (string "SELECT DISTINCT(page) FROM keywords where keyword is '"
                  (k "keyword") "';")))
    (org
     (string
      (string/replace-all "_" "&lowbar;" (k "keyword")) ": "))
    (ref ((pages 0) "page"))
    (for i 1 (length pages)
      (org ", ")
      (ref ((pages i) "page")))
    (org "\n"))
  (sqlite3/close db))
