exception UnexpectedExportType(string);

type id = string;

type file = {
  path: string,
  createdAt: Js.Date.t,
};

type exportType =
  | Teams
  | Students;

type t = {
  id,
  tags: array(string),
  createdAt: Js.Date.t,
  file: option(file),
  reviewedOnly: bool,
  exportType,
};

let id = t => t.id;
let createdAt = (t: t) => t.createdAt;
let tags = t => t.tags;
let file = t => t.file;
let exportType = t => t.exportType;
let reviewedOnly = t => t.reviewedOnly;
let fileCreatedAt = (file: file) => file.createdAt;
let filePath = file => file.path;

let decodeFile = json =>
  Json.Decode.{
    path: json |> field("path", string),
    createdAt: json |> field("createdAt", DateFns.decodeISO),
  };

let decode = json =>
  Json.Decode.{
    id: json |> field("id", string),
    createdAt: json |> field("createdAt", DateFns.decodeISO),
    file: json |> field("file", nullable(decodeFile)) |> Js.Null.toOption,
    tags: json |> field("tags", array(string)),
    exportType:
      switch (json |> field("exportType", string)) {
      | "Students" => Students
      | "Teams" => Teams
      | otherExportType =>
        Rollbar.error(
          "Unexpected exportType encountered: " ++ otherExportType,
        );
        raise(UnexpectedExportType(otherExportType));
      },
    reviewedOnly: json |> field("reviewedOnly", bool),
  };

let make = (~id, ~exportType, ~createdAt, ~tags, ~reviewedOnly) => {
  let finalTags =
    switch (exportType) {
    | Students => tags
    | Teams => [||]
    };

  {id, createdAt, tags: finalTags, exportType, reviewedOnly, file: None};
};

let exportTypeToString = t =>
  switch (t.exportType) {
  | Students => "Students"
  | Teams => "Teams"
  };
