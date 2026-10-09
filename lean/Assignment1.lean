/--
  CS F214 Assignment 1, Part B

  Lean versions of Examples 1.13 through 1.18 from Huth and Ryan,
  *Logic in Computer Science*.  `P`, `Q`, and `R` are propositions;
  `→`, `∧`, and `∨` are Lean's implication, conjunction, and disjunction.
/-

-- Example 1.13: P ∧ Q → R ⊢ P → (Q → R)
theorem example_1_13 (P Q R : Prop) : ((P ∧ Q) → R) → P → Q → R := by
  intro hImp hP hQ
  exact hImp ⟨hP, hQ⟩

-- Example 1.14: P → (Q → R) ⊢ P ∧ Q → R
theorem example_1_14 (P Q R : Prop) : (P → Q → R) → (P ∧ Q) → R := by
  intro hImp hAnd
  exact hImp hAnd.left hAnd.right

-- Example 1.15: P → Q ⊢ P ∧ R → Q ∧ R
theorem example_1_15 (P Q R : Prop) : (P → Q) → (P ∧ R) → (Q ∧ R) := by
  intro hImp hAnd
  exact ⟨hImp hAnd.left, hAnd.right⟩

-- Example 1.16: Q → R ⊢ P ∨ Q → P ∨ R
theorem example_1_16 (P Q R : Prop) : (Q → R) → (P ∨ Q) → (P ∨ R) := by
  intro hImp hOr
  cases hOr with
  | inl hP => exact Or.inl hP
  | inr hQ => exact Or.inr (hImp hQ)

-- Example 1.17: (P ∨ Q) ∨ R ⊢ P ∨ (Q ∨ R)
theorem example_1_17 (P Q R : Prop) : ((P ∨ Q) ∨ R) → (P ∨ (Q ∨ R)) := by
  intro hOr
  cases hOr with
  | inl hPQ =>
      cases hPQ with
      | inl hP => exact Or.inl hP
      | inr hQ => exact Or.inr (Or.inl hQ)
  | inr hR => exact Or.inr (Or.inr hR)

-- Example 1.18: P ∧ (Q ∨ R) ⊢ (P ∧ Q) ∨ (P ∧ R)
theorem example_1_18 (P Q R : Prop) : (P ∧ (Q ∨ R)) → ((P ∧ Q) ∨ (P ∧ R)) := by
  intro hAnd
  cases hAnd.right with
  | inl hQ => exact Or.inl ⟨hAnd.left, hQ⟩
  | inr hR => exact Or.inr ⟨hAnd.left, hR⟩
