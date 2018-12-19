
class Dumper : public clang::RecursiveASTVisitor<Dumper> {
  typedef clang::RecursiveASTVisitor<Dumper> Super;
public:
  explicit Dumper (
      clang::ASTContext& Context,
      clang::SourceManager& SourceManager,
      clang::FileID userFileID) :
    Context(Context),
    SourceManager(SourceManager),
    userFileID(userFileID),
    root(new Json::Value(Json::arrayValue)),
    node(root),
    LangOpts(),
    pp(LangOpts),
    nextId(0)
  {
    (*node)[0] = "Root";
    (*node)[1] = Json::Value(Json::objectValue);
    (*node)[2] = Json::Value(Json::arrayValue);
  }

  Json::Value& getTranslationUnit() const {
    return (*root)[2][0];
  }

  bool shouldVisitImplicitCode() const {
    return true;
  }

  void BeginNode(char const * kind) {
    parents.push_back(std::move(node));
    node.reset(new Json::Value(Json::arrayValue));
    (*node)[0] = kind;
    (*node)[1] = Json::Value(Json::objectValue);
    (*node)[1]["id"] = nextId;
    nextId++;
    (*node)[2] = Json::Value(Json::arrayValue);
  }

  void EndNode() {
    std::unique_ptr<Json::Value> child(std::move(node));
    node = std::move(parents.back());
    parents.pop_back();
    (*node)[2].append(*child);
  }

  bool TraverseDecl (clang::Decl *D) {
    if (D != nullptr) {
      std::string kind(D->getDeclKindName());
      kind += "Decl";
      BeginNode(kind.c_str());
      setSourceRange(D->getSourceRange());
      if (D->isImplicit())
        (*node)[1]["implicit"] = true;
      Super::TraverseDecl(D);
      EndNode();
    }
    return true;
  }
  bool TraverseStmt (clang::Stmt *S) {
    if (S != nullptr) {
      BeginNode(S->getStmtClassName());
      setSourceRange(S->getSourceRange());
      Super::TraverseStmt(S);
      EndNode();
    }
    return true;
  }
  bool TraverseType (clang::QualType T) {
    if (!T.isNull()) {
      std::string kind(T->getTypeClassName());
      kind += "Type";
      BeginNode(kind.c_str());
      Super::TraverseType(T);
      EndNode();
    }
    return true;
  }
  bool TraverseTypeLoc(clang::TypeLoc TL) {
    if (!TL.isNull()) {
      std::string kind(TL.getTypePtr()->getTypeClassName());
      kind += "Type";
      BeginNode(kind.c_str());
      // TODO: add location to (*node)[1]
      Super::TraverseTypeLoc(TL);
      EndNode();
    }
    return true;
  }
  bool TraverseDeclarationNameInfo(clang::DeclarationNameInfo NameInfo) {
    BeginNode("Name");
    setSourceRange(NameInfo.getSourceRange());
    // In C all names are simple identifiers.
    Super::TraverseDeclarationNameInfo(NameInfo);
    auto IdentInfo = NameInfo.getName().getAsIdentifierInfo();
    if (IdentInfo) {
      (*node)[1]["identifier"] = IdentInfo->getName().str();
    }
    EndNode();
    return true;
  }
  bool TraverseTypedefDecl(clang::TypedefDecl *Decl) {
    Super::TraverseTypedefDecl(Decl);
    // Traverse the declared type, normally omitted by clang.
    TraverseType(clang::QualType(Decl->getTypeForDecl(), 0));
  }
  bool TraverseImplicitCastExpr(clang::ImplicitCastExpr *Expr) {
    Super::TraverseImplicitCastExpr(Expr);
    // Add traversal of the cast type, normally omitted by clang.
    TraverseType(Expr->getType());
    return true;
  }
  bool TraverseSynOrSemInitListExpr(clang::InitListExpr *S, clang::RecursiveASTVisitor<Dumper>::DataRecursionQueue *Q) {
    // Limit traversal to the semantic form of the initializer.
    if (S && S->isSemanticForm()) {
      return Super::TraverseSynOrSemInitListExpr(S, Q);
    }
    return true;
  }

  // Types
  bool VisitFunctionProtoType(clang::FunctionProtoType *Type) {
    if (Type->isVariadic())
      (*node)[1]["variadic"] = true;
    return true;
  }
  bool VisitRecordType(clang::RecordType *Type) {
    clang::RecordDecl * Decl = Type->getDecl();
    (*node)[1]["name"] = Decl->getName().str();
    return true;
  }

  // Declarations
  bool VisitTranslationUnitDecl(const clang::TranslationUnitDecl *Decl) {
    return true;
  }
  bool VisitEmptyDecl(const clang::EmptyDecl *Decl) {
    return true;
  }
  bool VisitTypedefDecl(const clang::TypedefDecl *Decl) {
    (*node)[1]["name"] = Decl->getName().str();
    return true;
  }
  bool VisitFunctionDecl(const clang::FunctionDecl *Decl) {
    if (Decl->isVariadic())
      (*node)[1]["variadic"] = true;
    if (Decl->isThisDeclarationADefinition())
      (*node)[1]["define"] = true;
    if (Decl->isMain())
      (*node)[1]["main"] = true;
    return true;
  }
  bool VisitParmVarDecl(const clang::ParmVarDecl *Decl) {
    (*node)[1]["name"] = Decl->getName().str();
    return true;
  }
  bool VisitVarDecl(const clang::VarDecl *Decl) {
    (*node)[1]["name"] = Decl->getName().str();
    return true;
  }
  // bool VisitTypeAliasDecl(const clang::TypeAliasDecl *Decl) { return true; }
  // bool VisitEnumDecl(const clang::EnumDecl *Decl) { return true; }
  bool VisitRecordDecl(const clang::RecordDecl *Decl) {
    (*node)[1]["name"] = Decl->getName().str();
    return true;
  }
  bool VisitFieldDecl(const clang::FieldDecl *Decl) {
    (*node)[1]["name"] = Decl->getName().str();
    return true;
  }
  // bool VisitEnumConstantDecl(const clang::EnumConstantDecl *Decl) { return true; };
  // bool VisitLabelDecl(const clang::LabelDecl *Decl) { return true; }

  // Statements
  bool VisitNullStmt (const clang::NullStmt *Stmt) {
    return true;
  }
  bool VisitDeclStmt (const clang::DeclStmt *Stmt) {
    return true;
  }
  bool VisitCompoundStmt (const clang::CompoundStmt *Stmt) {
    return true;
  }
  bool VisitReturnStmt (const clang::ReturnStmt *Stmt) {
    return true;
  }
  bool VisitIfStmt (const clang::IfStmt *Stmt) {
    return true;
  }
  bool VisitForStmt (const clang::ForStmt *Stmt) {
    if (!Stmt->getInit()) {
      (*node)[1]["noInit"] = true;
    }
    if (!Stmt->getCond()) {
      (*node)[1]["noCond"] = true;
    }
    if (!Stmt->getInc()) {
      (*node)[1]["noInc"] = true;
    }
    return true;
  }
  bool VisitDoStmt (const clang::DoStmt *Stmt) {
    return true;
  }
  bool VisitWhileStmt (const clang::WhileStmt *Stmt) {
    return true;
  }
  bool VisitBreakStmt (const clang::BreakStmt *Stmt) {
    return true;
  }
  bool VisitContinueStmt (const clang::ContinueStmt *Stmt) {
    return true;
  }
  bool VisitSwitchStmt (const clang::SwitchStmt *Stmt) {
    return true;
  }
  bool VisitSwitchCase (const clang::SwitchCase *Stmt) {
    return true;
  }
  bool VisitDefaultStmt (const clang::DefaultStmt *Stmt) {
    return true;
  }
  // bool VisitLabelStmt (const clang::LabelStmt *Stmt) { return true; };
  // bool VisitGotoStmt (const clang::GotoStmt *Stmt) { return true; };

  // Expressions
  bool VisitIntegerLiteral (const clang::IntegerLiteral *Expr) {
    auto t = static_cast<const clang::BuiltinType *>(Expr->getType().getTypePtr());
    bool isUnsigned = t->isUnsignedInteger();
    std::string repr = Expr->getValue().toString(10, !isUnsigned);
    if (isUnsigned)
      repr.append("u");
    (*node)[1]["value"] = repr;
    return true;
  }
  bool VisitFloatingLiteral (const clang::FloatingLiteral *Expr) {
    llvm::SmallString<32> StrVal;
    Expr->getValue().toString(StrVal, 0, 0);
    if (&Expr->getSemantics() == &Context.getTargetInfo().getFloatFormat()) {
      // Append 'f' to float literals.
      StrVal += 'f';
    }
    (*node)[1]["value"] = StrVal.str().str();
    return true;
  }
  bool VisitCharacterLiteral (const clang::CharacterLiteral *Expr) {
    (*node)[1]["value"] = Expr->getValue();
    char const * kind = 0;
    switch (Expr->getKind()) {
    case clang::CharacterLiteral::Ascii: kind = "Ascii"; break;
    case clang::CharacterLiteral::Wide: kind = "Wide"; break;
    case clang::CharacterLiteral::UTF8: kind = "UTF8"; break;
    case clang::CharacterLiteral::UTF16: kind = "UTF16"; break;
    case clang::CharacterLiteral::UTF32: kind = "UTF32"; break;
    }
    (*node)[1]["kind"] = kind;
    return true;
  }
  bool VisitStringLiteral (const clang::StringLiteral *Expr) {
    (*node)[1]["value"] = Expr->getBytes().str();
    return true;
  }
  bool VisitParenExpr (const clang::ParenExpr *Expr) {
    return true;
  }
  bool VisitMemberExpr (const clang::MemberExpr *Expr) {
    if (Expr->isArrow()) {
      (*node)[1]["isArrow"] = true;
    }
    return true;
  }
  bool VisitUnaryOperator (const clang::UnaryOperator *Expr) {
    char const * name = 0;
    switch (Expr->getOpcode()) {
    case clang::UO_PostInc: name = "PostInc"; break;
    case clang::UO_PostDec: name = "PostDec"; break;
    case clang::UO_PreInc: name = "PreInc"; break;
    case clang::UO_PreDec: name = "PreDec"; break;
    case clang::UO_AddrOf: name = "AddrOf"; break;
    case clang::UO_Deref: name = "Deref"; break;
    case clang::UO_Plus: name = "Plus"; break;
    case clang::UO_Minus: name = "Minus"; break;
    case clang::UO_Not: name = "Not"; break;
    case clang::UO_LNot: name = "LNot"; break;
    case clang::UO_Real: name = "Real"; break;
    case clang::UO_Imag: name = "Imag"; break;
    case clang::UO_Extension: name = "Extension"; break;
    }
    (*node)[1]["opcode"] = name;
    return true;
  }
  bool VisitBinaryOperator (const clang::BinaryOperator *Expr) {
    char const * name = 0;
    switch (Expr->getOpcode()) {
    case clang::BO_PtrMemD: name = "PtrMemD"; break;
    case clang::BO_PtrMemI: name = "PtrMemI"; break;
    case clang::BO_Mul: name = "Mul"; break;
    case clang::BO_Div: name = "Div"; break;
    case clang::BO_Rem: name = "Rem"; break;
    case clang::BO_Add: name = "Add"; break;
    case clang::BO_Sub: name = "Sub"; break;
    case clang::BO_Shl: name = "Shl"; break;
    case clang::BO_Shr: name = "Shr"; break;
    case clang::BO_LT: name = "LT"; break;
    case clang::BO_GT: name = "GT"; break;
    case clang::BO_LE: name = "LE"; break;
    case clang::BO_GE: name = "GE"; break;
    case clang::BO_EQ: name = "EQ"; break;
    case clang::BO_NE: name = "NE"; break;
    case clang::BO_And: name = "And"; break;
    case clang::BO_Xor: name = "Xor"; break;
    case clang::BO_Or: name = "Or"; break;
    case clang::BO_LAnd: name = "LAnd"; break;
    case clang::BO_LOr: name = "LOr"; break;
    case clang::BO_Assign: name = "Assign"; break;
    case clang::BO_MulAssign: name = "MulAssign"; break;
    case clang::BO_DivAssign: name = "DivAssign"; break;
    case clang::BO_RemAssign: name = "RemAssign"; break;
    case clang::BO_AddAssign: name = "AddAssign"; break;
    case clang::BO_SubAssign: name = "SubAssign"; break;
    case clang::BO_ShlAssign: name = "ShlAssign"; break;
    case clang::BO_ShrAssign: name = "ShrAssign"; break;
    case clang::BO_AndAssign: name = "AndAssign"; break;
    case clang::BO_OrAssign: name = "OrAssign"; break;
    case clang::BO_XorAssign: name = "XorAssign"; break;
    case clang::BO_Comma: name = "Comma"; break;
    }
    (*node)[1]["opcode"] = name;
    return true;
  }
  bool VisitCompoundAssignOperator (const clang::CompoundAssignOperator *Expr) {
    // TODO?
    return true;
  }
  bool VisitCallExpr (const clang::CallExpr *Expr) {
    return true;
  }
  bool VisitBlockExpr (const clang::BlockExpr *Expr) {
    return true;
  }
  bool VisitMaterializeTemporaryExpr (const clang::MaterializeTemporaryExpr *Expr) {
    return true;
  }
  bool VisitImplicitValueInitExpr (const clang::ImplicitValueInitExpr *Expr) {
    return true;
  }
  bool VisitArraySubscriptExpr (const clang::ArraySubscriptExpr *Expr) {
    return true;
  }
  bool VisitInitListExpr (const clang::InitListExpr *Expr) {
    return true;
  }
  bool VisitAbstractConditionalOperator (const clang::AbstractConditionalOperator *Expr) {
    return true;
  }
  bool VisitCastExpr (const clang::CastExpr *Expr) {
    return true;
  }
  bool VisitImplicitCastExpr (const clang::ImplicitCastExpr *Expr) {
    return true;
  }
  bool VisitTypoExpr (const clang::TypoExpr *Expr) {
    return true;
  }
  bool VisitCXXConstructExpr (const clang::CXXConstructExpr *Expr) {
    auto Constructor = Expr->getConstructor();
    /* (*node)[1]["declId"] = …  TODO: find id of construtor decl */
    return true;
  }
  // bool VisitParenListExpr (const clang::ParenListExpr *Expr) { return true; };
  // bool VisitNoInitExpr (const clang::NoInitExpr *Expr) { return true; };
  // bool VisitPredefinedExpr (const clang::PredefinedExpr *Expr) { return true; };
  // bool VisitAddrLabelExpr (const clang::AddrLabelExpr *Expr) { return true; };
  // bool VisitVAArgExpr (const clang::VAArgExpr *Expr) { return true; };

  bool VisitBuiltinType(const clang::BuiltinType *Type) {
    (*node)[1]["name"] = Type->getNameAsCString(pp);
    return true;
  }

  bool VisitConstantArrayType (const clang::ConstantArrayType *T) {
    llvm::APInt Size(T->getSize());
    (*node)[1]["size"] = Size.toString(10, false);
    return true;
  };

  void setSourceRange(const clang::SourceRange & R) {
    if (R.isInvalid()) {
      return;
    }
    /* Only add locations for STDIN */
    auto fileID = SourceManager.getFileID(R.getBegin());
    if (userFileID != fileID) {
      return;
    }
    auto SourceR = clang::CharSourceRange::getTokenRange(R);
    auto FileR = clang::Lexer::makeFileCharRange(SourceR, SourceManager, LangOpts);
    if (FileR.isValid()) {
      (*node)[1]["begin"] = SourceManager.getFileOffset(FileR.getBegin());
      (*node)[1]["end"] = SourceManager.getFileOffset(FileR.getEnd());
    }
  }

  Json::Value *root;
private:
  unsigned nextId;
  clang::ASTContext& Context;
  clang::SourceManager& SourceManager;
  clang::LangOptions LangOpts;
  clang::PrintingPolicy pp;
  std::unique_ptr<Json::Value> node;
  std::vector<std::unique_ptr<Json::Value>> parents;
  clang::FileID userFileID;
};

struct ParseTranslationUnitInfo {
  llvm::MemoryBuffer * SourceBuffer;
  llvm::MemoryBuffer * WrapperBuffer;
  char const * sysroot;
};

void parseTranslationUnit(void *data) {
  ParseTranslationUnitInfo *PTUI = static_cast<ParseTranslationUnitInfo*>(data);

  // Args.
  std::unique_ptr<std::vector<const char *>> Args(
      new std::vector<const char *>());
  llvm::CrashRecoveryContextCleanupRegistrar<std::vector<const char*> >
    ArgsCleanup(Args.get());
  Args->push_back("-fspell-checking");
  Args->push_back("-x");
  Args->push_back(::getenv("LANGUAGE"));
  Args->push_back("-Wuninitialized");
  if (PTUI->sysroot) {
    Args->push_back("--sysroot");
    Args->push_back(PTUI->sysroot);
  }
  // Args->push_back("-Xclang");

  // File remapping.
  std::unique_ptr<std::vector<clang::ASTUnit::RemappedFile>> RemappedFiles(
      new std::vector<clang::ASTUnit::RemappedFile>());
  llvm::CrashRecoveryContextCleanupRegistrar<
    std::vector<clang::ASTUnit::RemappedFile> > RemappedCleanup(RemappedFiles.get());
  char const * userFileName = "/user-input";
  char const * mainFileName = userFileName;
  if (PTUI->WrapperBuffer) {
    mainFileName = "wrapper";
    RemappedFiles->push_back(std::make_pair(mainFileName, PTUI->WrapperBuffer));
  }
  RemappedFiles->push_back(std::make_pair(userFileName, PTUI->SourceBuffer));
  Args->push_back(mainFileName);

  std::shared_ptr<clang::PCHContainerOperations> PCHContainerOps(
    new clang::PCHContainerOperations());

  // Diagnostics.
  llvm::IntrusiveRefCntPtr<clang::DiagnosticOptions> DiagOpts = new clang::DiagnosticOptions();
  DiagOpts->ShowColors = true;
  clang::TextDiagnosticPrinter *DiagClient =
    new clang::TextDiagnosticPrinter(llvm::errs(), &*DiagOpts);
  llvm::IntrusiveRefCntPtr<clang::DiagnosticsEngine> Diags(
    clang::CompilerInstance::createDiagnostics(&*DiagOpts, DiagClient));
  llvm::CrashRecoveryContextCleanupRegistrar<clang::DiagnosticsEngine,
    llvm::CrashRecoveryContextReleaseRefCleanup<clang::DiagnosticsEngine> >
    DiagCleanup(Diags.get());
  unsigned NumErrors = Diags->getClient()->getNumErrors();

  std::unique_ptr<clang::ASTUnit> ErrUnit;
  std::unique_ptr<clang::ASTUnit> Unit(clang::ASTUnit::LoadFromCommandLine(
    /*ArgBegin*/ Args->data(),
    /*ArgEnd*/ Args->data() + Args->size(),
    /*PCHContainerOps*/ PCHContainerOps,
    /*DiagnosticsEngine*/ Diags,
    /*ResourceFilesPath*/ llvm::StringRef(),
    /*OnlyLocalDecls*/ false,
    /*CaptureDiagnostics*/ false,
    /*RemappedFiles*/ *RemappedFiles.get(),
    /*RemappedFilesKeepOriginalName*/ true,
    /*PrecompilePreamble*/ 0,
    /*TranslationUnitKind*/ clang::TU_Complete,
    /*CacheCodeCompletionResults*/ false,
    /*IncludeBriefCommentsInCodeCompletion*/ false,
    /*AllowPCHWithCompilerErrors=*/ false,
    /*SkipFunctionBodies*/ clang::SkipFunctionBodiesScope::None,
    /*SingleFileParse*/ true,
    /*UserFilesAreVolatile*/ true,
    /*ForSerialization*/ false,
    llvm::None,
    &ErrUnit));

  if (NumErrors == Diags->getClient()->getNumErrors()) {
    clang::ASTContext& Context(Unit->getASTContext());
    clang::SourceManager& SM(Unit->getSourceManager());
    auto userFileEntry = SM.getFileManager().getFile(userFileName);
    auto userFileID = SM.getOrCreateFileID(userFileEntry, clang::SrcMgr::C_User);
    auto dumper = Dumper(Context, SM, userFileID);
    dumper.TraverseDecl(
      static_cast<clang::Decl*>(Context.getTranslationUnitDecl()));
    std::unique_ptr<Json::FastWriter> writer(new Json::FastWriter());
    writer->omitEndingLineFeed();
    std::cout << writer->write(dumper.getTranslationUnit()) << std::endl;
  }

  ErrUnit.release();
  Unit.release();
}

int main(int argc, char const * const * argv) {

  ParseTranslationUnitInfo PTUI;

  PTUI.sysroot = ::getenv("SYSROOT");

  auto SourceBuffer = llvm::MemoryBuffer::getSTDIN();
  if (std::error_code ec = SourceBuffer.getError()) {
    std::cerr << "error reading file" << std::endl;
    return 1;
  }
  PTUI.SourceBuffer = SourceBuffer->release();

  char const * wrapper = ::getenv("SOURCE_WRAPPER");
  if (wrapper) {
    auto WrapperBuffer = llvm::MemoryBuffer::getFile(wrapper);
    if (std::error_code ec = WrapperBuffer.getError()) {
      std::cerr << "error reading wrapper" << std::endl;
      return 1;
    }
    PTUI.WrapperBuffer = WrapperBuffer->release();
  } else {
    PTUI.WrapperBuffer = 0;
  }

  llvm::CrashRecoveryContext CRC;
  unsigned size = 8<<20;  // 8 MB
  if (!CRC.RunSafely(parseTranslationUnit, &PTUI)) {
    std::cerr << "crashed" << std::endl;
    return 1;
  }

  return 0;
}
