BEGIN {
  ns=""
  top_ns=""
}

$2 == "{" {
  if (top_ns == "") {
    top_ns = $1
  }
  else if (ns == "") {
    ns = $1
  count = 0
  }
  else {
    printf ("Unexpected occurrence of '{' inside a namespace: %s:%d\n",
	    FILENAME, FNR)
    exit 1
  }

  next
}

$1 == "}" {
  if (ns != "") {
    ns = ""
  }
  else if (top_ns != "") {
    top_ns = ""
  }
  else {
    printf ("syntax error: extra }: %s:%d\n", FILENAME, FNR)
    exit 1
  }
  next
}

{
  if (ns == "") {
    print "Invalid tunable outside a namespace"
    exit 1
  }
  val[top_ns][ns][count] = $1
  count = count + 1
}

END {
  if (ns != "") {
    print "Unterminated namespace.  Is a closing brace missing?"
    exit 1
  }

  print "typedef enum"
  print "{"
  for (t in val) {
    for (n in val[t]) {
      printf ("  %s_%s,\n", t, n);
      for (c in val[t][n]) {
        printf ("  %s_%s_%s,\n", t, n, val[t][n][c]);
      }
    }
  }
  print "  TUNABLES_MAX"
  print "} tunable_id_t;"
}
