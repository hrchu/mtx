

/* MySQL table format. */
create table hosts (
  osname text,
  osversion text
);

create table loaders (
     enabled integer,  /* 0=no, 1=yes */  

     worked integer, /* 0=no, 1=yes */
     osname text,
     osversion text,
 
     description text,
     /* loaderinfo output */
     vendorid text,
     productid text,
     revision text,
     barcodes integer,
     eaap integer,
     transports integer,
     slots integer,
     imports integer,
     transfers integer,
     tgdp integer,
     canxfer integer,
    
     serialnum text,
    
     email text,
     name text
);
     
     
